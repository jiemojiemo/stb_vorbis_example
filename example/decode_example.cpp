//
// Created by william on 2019/12/1.
//
#define STB_DEFINE
#define STB_ONLY
#include "stb.h"           /*  http://nothings.org/stb.h  */
#include "stb_vorbis.c"
#include "AudioFile.h"
#include <string>

using namespace std;

void decode_file(const string& ogg_path, const string& output_path)
{
    int num_channels = 0;
    int sample_rate = 0;

    short *decoded = nullptr;
    int len = stb_vorbis_decode_filename(ogg_path.c_str(), &num_channels, &sample_rate, &decoded);

    if(len)
    {
        cout << len << endl;
        AudioFile<float> output_file;
        output_file.setNumChannels(num_channels);
        output_file.setSampleRate(sample_rate);
        output_file.setNumSamplesPerChannel(len);

        for(int i = 0; i < len; ++i)
        {
            for(int c = 0; c < num_channels; ++c)
            {
                output_file.samples[c][i] = decoded[i*num_channels + c] / 32768.0;
            }
        }

        output_file.save(output_path);
    }
}
void decode_file_frame_by_frame_short(const string& filename, const string& output_path)
{
    short sbuffer[8000];
    int n = 0;
    int err = 0;
    stb_vorbis *v = stb_vorbis_open_filename(filename.c_str(), &err, NULL);
    if(!v)
    {
        stb_fatal("Couldn't open {%s}", filename.c_str());
    }

    int num_channels = v->channels;
    int sample_rate = v->sample_rate;
    AudioFile<float> output_file;
    output_file.setNumChannels(num_channels);
    output_file.setSampleRate(sample_rate);

    for(;;)
    {
        n = stb_vorbis_get_frame_short_interleaved(v, num_channels, sbuffer, 8000);

        // decode frame successfully
        if(n == 0)
            break;

        for(int i = 0; i < n; ++i)
        {
            for(int c = 0; c < num_channels; ++c)
            {
                output_file.samples[c].push_back( sbuffer[i*num_channels + c]/32768.0 );
            }
        }
    }

    output_file.save(output_path);
    stb_vorbis_close(v);
}

void decode_file_frame_by_frame_float(const string& filename, const string& output_path)
{
    int n = 0;
    int err = 0;
    stb_vorbis *v = stb_vorbis_open_filename(filename.c_str(), &err, NULL);
    if(!v)
    {
        stb_fatal("Couldn't open {%s}", filename.c_str());
    }

    int num_channels = v->channels;
    int sample_rate = v->sample_rate;
    AudioFile<float> output_file;
    output_file.setNumChannels(num_channels);
    output_file.setSampleRate(sample_rate);

    float** fbuffer;
    for(;;)
    {
        n = stb_vorbis_get_frame_float(v, &num_channels, &fbuffer);

        // decode frame successfully
        if(n == 0)
            break;

        for(int i = 0; i < n; ++i)
        {
            for(int c = 0; c < num_channels; ++c)
            {
                output_file.samples[c].push_back(fbuffer[c][i]);
            }
        }
    }

    output_file.save(output_path);
    stb_vorbis_close(v);
}

void decode_file_samples_by_samples(const string& filename, const string& output_path)
{
    int error;
    stb_vorbis *v = stb_vorbis_open_filename(filename.c_str(), &error, NULL);
    if (!v) stb_fatal("Couldn't open {%s}", filename.c_str());

    int num_channels = v->channels;
    int sample_rate = v->sample_rate;
    AudioFile<float> output_file;
    output_file.setNumChannels(num_channels);
    output_file.setSampleRate(sample_rate);

    int16 sbuffer[233];
    for(;;)
    {
        int n = stb_vorbis_get_samples_short_interleaved(v, num_channels, sbuffer, 233);
        if(n == 0)
            break;

        for(int i = 0; i < n; ++i)
        {
            for(int c = 0; c < num_channels; ++c)
            {
                output_file.samples[c].push_back(sbuffer[i*num_channels + c] / 32768.0);
            }
        }
    }

    output_file.save(output_path);
    stb_vorbis_close(v);
}

void seek_to_sample(const string& filename, const string& output_path)
{
    int count = 0;
    short samples[1024];

    int error = 0;
    stb_vorbis *v = stb_vorbis_open_filename(filename.c_str(), &error, NULL);
    if (!v) stb_fatal("Couldn't open {%s}", filename.c_str());

    const int num_channels = v->channels;
    const int sample_rate = v->sample_rate;

    const int seek_pos = sample_rate * 5.0; // seek to 5.0s

    if(!stb_vorbis_seek(v, seek_pos)){
        fprintf(stderr, "Seek to %u returned error from stb_vorbis\n", seek_pos);
        return;
    }

    AudioFile<float> output_file;
    output_file.setNumChannels(num_channels);
    output_file.setSampleRate(sample_rate);

    for(;;)
    {
        int n = stb_vorbis_get_samples_short_interleaved(v, num_channels, samples, 1024);
        if(n == 0)
            break;

        for(int i = 0; i < n; ++i)
        {
                for(int c = 0; c < num_channels; ++c)
                {
                    output_file.samples[c].push_back(samples[i*num_channels + c] / 32768.0);
            }
        }

        if(n != 1024 && n == 0)
            break;
    }

    stb_vorbis_close(v);
    output_file.save(output_path);
};

int main()
{
    string filename = "../data/gs-16b-2c-44100hz.ogg";
    string output_path = "ogg_out.wav";

    decode_file(filename, "decode_file.wav");
    decode_file_frame_by_frame_float(filename, "decode_frame.wav");
    decode_file_samples_by_samples(filename, "decode_sample.wav");
    seek_to_sample(filename, "seek_file.wav");
}