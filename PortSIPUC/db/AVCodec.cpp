#include "stdafx.h"
#include "AVCodec.h"
CAVCodec::CAVCodec()
{
	m_nAVCodecID = 0;
	m_nItemIndex = 0 ;
	m_nCodecCode = 0;
	m_nAVCodecType= ENUM_VA_CODEC_VIDEO;
	m_strCodecName="";
	m_strReserved="";
	m_bAVCodecOpen = false;
}
CAVCodec::~CAVCodec()
{

}