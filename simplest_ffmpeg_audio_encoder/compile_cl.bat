::��򵥵Ļ���FFmpeg����Ƶ������----�����б���
::Simplest FFmpeg Audio Encoder----Compile in Cmd 
::
::������ Lei Xiaohua
::leixiaohua1020@126.com
::�й���ý��ѧ/���ֵ��Ӽ���
::Communication University of China / Digital TV Technology
::http://blog.csdn.net/leixiaohua1020
::
::include
@set INCLUDE=include;%INCLUDE%;
::lib
@set LIB=lib;%LIB%;
::compile and link
cl simplest_ffmpeg_audio_encoder.cpp /link avcodec.lib avformat.lib avutil.lib ^
avdevice.lib avfilter.lib postproc.lib swresample.lib swscale.lib /OPT:NOREF