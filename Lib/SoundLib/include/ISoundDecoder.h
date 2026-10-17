#ifndef __ISOUNDDECODER_H__
#define __ISOUNDDECODER_H__

class ISoundDecoder
{
public:
	virtual      ~ISoundDecoder()                                    {}
	virtual int  Decode(short* outbuf, int nbSample, bool loop)=0;
	virtual void GoTo(int sample)=0;
	virtual int  GetNumSamplesPerDecode(int neededSamplesPerDecode)  { return neededSamplesPerDecode; }
	virtual int  GetSamplingRate() const                             { return -1; };
	virtual int  GetChannel() const                                  { return -1; };
	virtual int  GetPCMSize() const                                  { return -1; };
	virtual int  GetNbEffectiveSamples() const                       { return -1; };
	virtual int  GetTotalSamples() const                             { return -1; };
};

#endif //__ISOUNDDECODER_H__
