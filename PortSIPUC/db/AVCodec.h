#ifndef AVCODEC_H
#define AVCODEC_H
#include "DBParam.h"
class CAVCodec
{
public:
	CAVCodec();
	~CAVCodec();

	int					m_nAVCodecID;
	int             	m_nItemIndex;
	int                 m_nCodecCode;
	ENUM_AV_CODEC					m_nAVCodecType;
	bool                m_bAVCodecOpen;
	std::string			m_strCodecName;
	std::string			m_strReserved;
};
#endif