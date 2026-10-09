#pragma once

#include <nw/snd/snd_Config.h>

#include <nw/snd/snd_BankFile.h>
#include <nw/snd/snd_BankFileReader.h>
#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_BiquadFilterCallback.h>
#include <nw/snd/snd_BiquadFilterPresets.h>
#include <nw/snd/snd_CurveAdshr.h>
#include <nw/snd/snd_CurveLfo.h>
#include <nw/snd/snd_DriverCommand.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_ExternalSoundPlayer.h>
#include <nw/snd/snd_FrameHeap.h>
#include <nw/snd/snd_FxBase.h>
#include <nw/snd/snd_FxDelay.h>
#include <nw/snd/snd_FxReverb.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_GroupFile.h>
#include <nw/snd/snd_GroupFileReader.h>
#include <nw/snd/snd_InstancePool.h>
#include <nw/snd/snd_ItemType.h>
#include <nw/snd/snd_MemorySoundArchive.h>
#include <nw/snd/snd_MoveValue.h>
#include <nw/snd/snd_PlayerHeap.h>
#include <nw/snd/snd_PlayerHeapDataManager.h>
#include <nw/snd/snd_RomSoundArchive.h>
#include <nw/snd/snd_SequenceSound.h>
#include <nw/snd/snd_SequenceSoundFile.h>
#include <nw/snd/snd_SequenceSoundFileReader.h>
#include <nw/snd/snd_SequenceSoundHandle.h>
#include <nw/snd/snd_Sound3DActor.h>
#include <nw/snd/snd_Sound3DCalculator.h>
#include <nw/snd/snd_Sound3DEngine.h>
#include <nw/snd/snd_Sound3DListener.h>
#include <nw/snd/snd_Sound3DManager.h>
#include <nw/snd/snd_SoundActor.h>
#include <nw/snd/snd_SoundArchive.h>
#include <nw/snd/snd_SoundArchiveFile.h>
#include <nw/snd/snd_SoundArchiveFileReader.h>
#include <nw/snd/snd_SoundArchiveLoader.h>
#include <nw/snd/snd_SoundArchivePlayer.h>
#include <nw/snd/snd_SoundDataManager.h>
#include <nw/snd/snd_SoundHandle.h>
#include <nw/snd/snd_SoundHeap.h>
#include <nw/snd/snd_SoundInstanceManager.h>
#include <nw/snd/snd_SoundMemoryAllocatable.h>
#include <nw/snd/snd_SoundPlayer.h>
#include <nw/snd/snd_SoundStartable.h>
#include <nw/snd/snd_SoundSystem.h>
#include <nw/snd/snd_StreamSound.h>
#include <nw/snd/snd_StreamSoundFile.h>
#include <nw/snd/snd_StreamSoundFileLoader.h>
#include <nw/snd/snd_StreamSoundFileReader.h>
#include <nw/snd/snd_StreamSoundHandle.h>
#include <nw/snd/snd_Task.h>
#include <nw/snd/snd_TaskManager.h>
#include <nw/snd/snd_TaskThread.h>
#include <nw/snd/snd_ThreadStack.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_WaveArchiveFile.h>
#include <nw/snd/snd_WaveArchiveFileReader.h>
#include <nw/snd/snd_WaveFile.h>
#include <nw/snd/snd_WaveFileReader.h>
#include <nw/snd/snd_WaveSound.h>
#include <nw/snd/snd_WaveSoundFile.h>
#include <nw/snd/snd_WaveSoundFileReader.h>
#include <nw/snd/snd_WaveSoundHandle.h>

#include <nw/snd/snd_Bank.h>
#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_ChannelManager.h>
#include <nw/snd/snd_DisposeCallback.h>
#include <nw/snd/snd_DisposeCallbackManager.h>
#include <nw/snd/snd_MmlCommand.h>
#include <nw/snd/snd_MmlParser.h>
#include <nw/snd/snd_MmlSequenceTrack.h>
#include <nw/snd/snd_MmlSequenceTrackAllocator.h>
#include <nw/snd/snd_NoteOnCallback.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_SequenceTrack.h>
#include <nw/snd/snd_SequenceTrackAllocator.h>
#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_StreamBufferPool.h>
#include <nw/snd/snd_StreamSoundPlayer.h>
#include <nw/snd/snd_StreamTrack.h>
#include <nw/snd/snd_Voice.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_WaveSoundPlayer.h>

#ifdef __cplusplus

using namespace nw::snd;
using namespace nw::snd::internal;
using namespace nw::snd::internal::driver;

#endif
