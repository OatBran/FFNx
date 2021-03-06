#pragma once

#include <windows.h>
#include <DSound.h>
#include "../log.h"
#include "winamp.h"

class WinampPlugin {
private:
	HINSTANCE handle;
	bool open(LPCWSTR libFileNameW, char* libFileNameA);
protected:
	virtual LPCSTR procName() const = 0;
	virtual bool openModule(FARPROC procAddress) = 0;
	virtual void closeModule() = 0;
	inline HINSTANCE getHandle() const {
		return this->handle;
	}
public:
	WinampPlugin();
	virtual ~WinampPlugin();
	bool open(LPCWSTR libFileName);
	bool open(char* libFileName);
	void close();
};

class AbstractOutPlugin {
protected:
	WinampOutModule* mod;
public:
	AbstractOutPlugin() : mod(nullptr) {}
	virtual ~AbstractOutPlugin() {}
	inline WinampOutModule* getModule() const {
		return mod;
	}
	// volume stuff
	void setVolume(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void setPan(int pan);	    // from -127 to 127.. usually just call outMod->SetPan
	int getOutputTime() const;
	int getWrittenTime() const;
	void pause() const;
	void unPause() const;
	// This method is not part of the winamp plugin
	virtual void setTempo(int tempo) = 0;
};

class WinampOutPlugin : public WinampPlugin, public AbstractOutPlugin {
private:
	inline LPCSTR procName() const {
		return "winampGetOutModule";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampOutPlugin();
	virtual ~WinampOutPlugin();
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

class CustomOutPlugin : public AbstractOutPlugin {
private:
	static IDirectSoundBuffer* sound_buffer;
	static DWORD sound_buffer_size;
	static DWORD sound_write_pointer;
	static DWORD bytes_written;
	static DWORD prebuffer_size;
	static DWORD start_t;
	static DWORD offset_t;
	static DWORD paused_t;
	static DWORD last_pause_t;
	static DWORD last_stop_t;
	static int last_pause;
	static WAVEFORMATEX sound_format;
	static bool play_started;
	static bool clear_done;
	static int last_volume;

	static void Config(HWND hwndParent);
	static void About(HWND hwndParent);
	static void Init();
	static void Quit();
	static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static void Close();
	static bool playHelper();
	static int Write(char* buf, int len);
	static int canWriteHelper(DWORD &current_play_cursor);
	static int CanWrite();
	// More likely: 'in': Hey 'out' I finished my decoding, have you finished playing?
	static int IsPlaying();
	static int Pause(int pause);
	static void SetVolume(int volume);
	static void SetPan(int pan);
	static void Flush(int t);
	static int GetOutputTime();
	static int GetWrittenTime();
public:
	CustomOutPlugin();
	virtual ~CustomOutPlugin();
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};

class AbstractInPlugin {
protected:
	WinampInModule* mod;
	AbstractOutPlugin* outPlugin;

	void initModule(HINSTANCE dllInstance);
	void quitModule();
public:
	AbstractInPlugin(AbstractOutPlugin* outPlugin, WinampInModule* mod = nullptr);
	virtual ~AbstractInPlugin();

	int play(const char* fn);
	int play(const wchar_t* fn);
	void pause();			// pause stream
	void unPause();			// unpause stream
	int isPaused();			// ispaused? return 1 if paused, 0 if not
	void stop();				// stop (unload) stream

	// time stuff
	int getLength();			// get length in ms
	int getOutputTime();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void setOutputTime(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..
};

class WinampInPlugin : public WinampPlugin, public AbstractInPlugin {
private:
	inline LPCSTR procName() const {
		return "winampGetInModule2";
	}
	bool openModule(FARPROC procAddress);
	void closeModule();
public:
	WinampInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~WinampInPlugin();
};

class VgmstreamInPlugin : public AbstractInPlugin {
public:
	VgmstreamInPlugin(AbstractOutPlugin* outPlugin);
	virtual ~VgmstreamInPlugin();
};
