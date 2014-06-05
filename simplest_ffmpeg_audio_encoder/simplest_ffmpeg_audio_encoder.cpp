/* 
 *��򵥵Ļ���FFmpeg����Ƶ������
 *Simplest FFmpeg Audio Encoder
 *
 *������ Lei Xiaohua
 *leixiaohua1020@126.com
 *�й���ý��ѧ/���ֵ��Ӽ���
 *Communication University of China / Digital TV Technology
 *http://blog.csdn.net/leixiaohua1020
 *
 *������ʵ������ƵPCM�������ݱ���Ϊѹ��������MP3��WMA��AAC�ȣ���
 *����򵥵�FFmpeg��Ƶ���뷽��Ľ̡̳�
 *ͨ��ѧϰ�����ӿ����˽�FFmpeg�ı������̡�
 *This software encode PCM data to AAC bitstream.
 *It's the simplest audio encoding software based on FFmpeg. 
 *Suitable for beginner of FFmpeg 
 */

#include "stdafx.h"

extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
};

int _tmain(int argc, _TCHAR* argv[])
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* audio_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;

	uint8_t* frame_buf;
	AVFrame* frame;
	int size;

	FILE *in_file = fopen("tdjm.pcm", "rb");	//��ƵPCM�������� 
	int framenum=1000;	//��Ƶ֡��
	const char* out_file = "tdjm.aac";					//����ļ�·��


	av_register_all();

	//����1.���ʹ�ü�������
	pFormatCtx = avformat_alloc_context();
	//�¸�ʽ
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;


	//����2.�����Զ���һЩ
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	//fmt = pFormatCtx->oformat;

	//ע�����·��
	if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0)
	{
		printf("����ļ���ʧ�ܣ�\n");
		return -1;
	}

	audio_st = av_new_stream(pFormatCtx, 0);
	if (audio_st==NULL){
		return -1;
	}
	pCodecCtx = audio_st->codec;
	pCodecCtx->codec_id = fmt->audio_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
	pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
	pCodecCtx->sample_rate= 44100;
	pCodecCtx->channel_layout=AV_CH_LAYOUT_STEREO;
	pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
	pCodecCtx->bit_rate = 64000;  

	//�����ʽ��Ϣ
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec)
	{
		printf("û���ҵ����ʵı�������\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
	{
		printf("��������ʧ�ܣ�\n");
		return -1;
	}
	frame = avcodec_alloc_frame();
	frame->nb_samples= pCodecCtx->frame_size;
	frame->format= pCodecCtx->sample_fmt;
	
	size = av_samples_get_buffer_size(NULL, pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt, 1);
	frame_buf = (uint8_t *)av_malloc(size);
	avcodec_fill_audio_frame(frame, pCodecCtx->channels, pCodecCtx->sample_fmt,(const uint8_t*)frame_buf, size, 1);
	
	//д�ļ�ͷ
	avformat_write_header(pFormatCtx,NULL);

	AVPacket pkt;
	av_new_packet(&pkt,size);

	for (int i=0; i<framenum; i++){
		//����PCM
		if (fread(frame_buf, 1, size, in_file) < 0)
		{
			printf("�ļ���ȡ����\n");
			return -1;
		}else if(feof(in_file)){
			break;
		}
		frame->data[0] = frame_buf;  //�����ź�

		frame->pts=i*100;
		int got_frame=0;
		//����
		int ret = avcodec_encode_audio2(pCodecCtx, &pkt,frame, &got_frame);
		if(ret < 0)
		{
			printf("�������\n");
			return -1;
		}
		if (got_frame==1)
		{
			printf("����ɹ���%d֡��\n",i);
			pkt.stream_index = audio_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);
		}
	}
	
	//д�ļ�β
	av_write_trailer(pFormatCtx);

	//����
	if (audio_st)
	{
		avcodec_close(audio_st->codec);
		av_free(frame);
		av_free(frame_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	fclose(in_file);

	return 0;
}


