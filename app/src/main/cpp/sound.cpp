#include "inc/fmod.hpp"
#include <stdlib.h>
#include <unistd.h>
#include  "com_aserbao_androidcustomcamera_utils_VoiceUtils.h"

#include <jni.h>

#include <android/log.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"zph",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"zph",FORMAT,##__VA_ARGS__);

#define MODE_NORMAL 0
#define MODE_LUOLI 1
#define MODE_DASHU 2
#define MODE_JINGSONG 3
#define MODE_GAOGUAI 4
#define MODE_KONGLING 5

using namespace FMOD;

JNIEXPORT void JNICALL Java_com_aserbao_androidcustomcamera_utils_VoiceUtils_fix(JNIEnv *env,
		jclass jcls, jstring path_jstr, jint type) {
    //c+++思想，操作指针
    //声音引擎
	System *system;
    //声音
	Sound *sound;
	//数字处理（音效）
    DSP *dsp;

	//音乐轨道
    Channel *channel;
	//播放速度
    float frequency = 0;
    //音频地址
    const char* path_cstr = env->GetStringUTFChars(path_jstr, NULL);
    
    // 创建系统
    System_Create(&system);
    // 系统的初始化：参数1 最大音轨数，参数2:系统初始化标记
	system->init(32, FMOD_INIT_NORMAL, 0);

	//创建声音：参数1:声音路径，参数2:声音初始化标记，参数3: 参数4:声音指针
	system->createSound(path_cstr, FMOD_DEFAULT, 0, &sound);
    //播放声音：参数1:声音，参数2:音轨，参数3:控制 参数4:通道
    system->playSound(sound, 0, false, &channel);

	switch (type) {
        case MODE_NORMAL:
            //原生播放
            break;
        case MODE_LUOLI:
            //提升或者降低音调的一种音效
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            //设置音调的参数
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.8);
            //添加进到channel，添加进轨道
            channel->addDSP(0, dsp);
            break;
        case MODE_DASHU:
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8);
            channel->addDSP(0, dsp);
            break;
        case MODE_JINGSONG:
            // 惊悚音效是通过多个音轨叠加的。
            
            // 音轨1 大叔 
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.7);
            channel->addDSP(0, dsp);

            // 音轨2 回音 ECHO
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200); //回音延时
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 10); //回音衰减
            channel->addDSP(1, dsp);

            system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, 20) //颤抖
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.8) //颤抖
            channel->addDSP(2, dsp);
            break;

        case MODE_GAOGUAI:
            //修改声音的频率
            channel->getFrequency(&frequency);
            frequency = frequency * 2;
            channel->setFrequency(frequency);
            break;
        case MODE_KONGLING:
            // 回音 ECHO
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 300); //回音延时
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20); //回音衰减
            channel->addDSP(0, dsp);
            break;
        }
    bool playing = true;
	//等待播放完毕，单位是微妙
	while (playing) {
		channel->isPlaying(&playing); //如果播放完成了音轨会知道，并将playing改为false。
        usleep(1000*1000); //每个一秒
	}

    //释放资源
    env->ReleaseStringUTFChars(path_jstr, path_cstr);
	sound->release();
	system->close();
	system->release();
}
