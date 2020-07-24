#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

#define IDT_STORYBOARD			100

Animation::Animation()
	: m_pTargetControl(NULL)
	, m_pTargetWindow(NULL)
	, m_eAnimationTargetProperty(APT_NULL)
	, m_dwDuration(1000)
	, m_eFillBehavior(FB_HOLDEND)
	, m_eHandoffBehavior(HB_SNAPSHOT_AND_REPLACE)
	, m_bAutoReverse(false)
	, m_dwRepeatBehaviorTimes(1)
	, m_dwRepeatBehaviorDuration(0)
	, m_nBeginTime(0)
	, m_nSpeedRatio(100)
	, m_bCumulative(false)
	, m_bHasSetFrom(false)
	, m_bHasSetTo(false)
	, m_bFirstFrameShow(false)
	, m_bCurReverse(false)
{
}

Animation::~Animation()
{
}

int Animation::SetTargetCtrlID(UINT nCtrlID)
{
	_ASSERT(FALSE);
	return -1;
}

UINT Animation::GetTargetCtrlID() const
{
	_ASSERT(FALSE);
	return -1;
}

int Animation::SetTargetName(LPCTSTR lpszCtrlName)
{
	_ASSERT(FALSE);
	return -1;
}

LPCTSTR Animation::GetTargetName() const
{
	_ASSERT(FALSE);
	return NULL;
}

int Animation::SetTarget(CWLWnd *pTarget)
{
	m_pTargetControl = pTarget;
	return 0;
}

CWLWnd *Animation::GetTarget()
{
	return m_pTargetControl;
}

int Animation::SetTargetWindow(Window *pTarget)
{
	m_pTargetWindow = pTarget;
	return 0;
}

Window *Animation::GetTargetWindow()
{
	return m_pTargetWindow;
}

int Animation::SetTargetProperty(ANIMATION_TARGET_PROPERTY eAtp)
{
	m_eAnimationTargetProperty = eAtp;
	return 0;
}

ANIMATION_TARGET_PROPERTY Animation::GetTargetProperty() const
{
	return m_eAnimationTargetProperty;
}

int Animation::SetDuration(DWORD dwDuration)
{
	m_dwDuration = dwDuration;
	return 0;
}

DWORD Animation::GetDuration() const
{
	return m_dwDuration;
}

int Animation::SetFillBehavior(FILL_BEHAVIOR eFb)
{
	m_eFillBehavior = eFb;
	return 0;
}

FILL_BEHAVIOR Animation::GetFillBehavior() const
{
	return m_eFillBehavior;
}

int Animation::SetHandoffBehavior(HANDOFF_BEHAVIOR eHb)
{
	m_eHandoffBehavior = eHb;
	return 0;
}

HANDOFF_BEHAVIOR Animation::GetHandoffBehavior() const
{
	return m_eHandoffBehavior;
}

int Animation::AutoReverse(bool bReverse)
{
	m_bAutoReverse = bReverse;
	return 0;
}

bool Animation::IsAutoReverse() const
{
	return m_bAutoReverse;
}

int Animation::SetRepeatBehaviorTimes(DWORD nRepeat)
{
	m_dwRepeatBehaviorTimes = nRepeat;
	return 0;
}

DWORD Animation::GetRepeatBehaviorTimes() const
{
	return m_dwRepeatBehaviorTimes;
}

int Animation::SetRepeatBehaviorDuration(DWORD nRepeat)
{
	m_dwRepeatBehaviorDuration = nRepeat;
	return 0;
}

DWORD Animation::GetRepeatBehaviorDuration() const
{
	return m_dwRepeatBehaviorDuration;
}

int Animation::SetBeginTime(int nBegin)
{
	m_nBeginTime = nBegin;
	return 0;
}

int Animation::GetBeginTime() const
{
	return m_nBeginTime;
}

int Animation::SetSpeedRatio(int nRatio)
{
	m_nSpeedRatio = nRatio;
	return 0;
}

int Animation::GetSpeedRatio() const
{
	return m_nSpeedRatio;
}

int Animation::SetCumulative(bool bCumulative)
{
	m_bCumulative = bCumulative;
	return 0;
}

bool Animation::IsCumulative() const
{
	return m_bCumulative;
}

int Animation::SetFirstFrameShow(bool bShow)
{
	m_bFirstFrameShow = bShow;
	return 0;
}

bool Animation::IsFirstFrameShow() const
{
	return m_bFirstFrameShow;
}

int Animation::TimerBehaviorStrategy(HTIMER hTimer, DWORD *pdwCurTime, bool *pbEnd)
{
	if (hTimer == NULL || pdwCurTime == NULL || pbEnd == NULL)
	{
		return -1;
	}

	WLTIMERINFO *pTimer = (WLTIMERINFO *)hTimer;

	// 1. 如果动画支持逆序播放
	if (IsAutoReverse())
	{
		// 如果还没有逆序播放，则逆序播放
		if (!m_bCurReverse)
		{
			m_bCurReverse = true;
			pTimer->dwStartTime = *pdwCurTime;
		}
		// 如果已经逆序播放完
		else
		{
			m_bCurReverse = false;

			// 是否支持重复播放
			if (GetRepeatBehaviorDuration() == -1)
			{
				pTimer->dwStartTime = *pdwCurTime;
			}
			else
			{
				// 指向第一帧
				*pdwCurTime = pTimer->dwStartTime;

				*pbEnd = true;
				KillTimer(pTimer->hOwner, pTimer->uIDEvent);
			}
		}
	}
	else
	{
		// 是否支持重复播放
		if (GetRepeatBehaviorDuration() == -1)
		{
			pTimer->dwStartTime = *pdwCurTime;
		}
		else
		{
			*pbEnd = true;
			KillTimer(pTimer->hOwner, pTimer->uIDEvent);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

KeyFrame::KeyFrame()
	: m_eKeyFrame(KF_LINEAR)
	, m_bHasSetKeyTime(false)
	, m_bKeyTimeIsTime(true)
	, m_dwKeyTime(0)
	, m_bHasSetValue(false)
{
	m_ptKeySpline1.x = m_ptKeySpline1.y = 0;
	m_ptKeySpline2.x = m_ptKeySpline2.y = 0;
}

KeyFrame::~KeyFrame()
{

}

//////////////////////////////////////////////////////////////////////////

DataKeyFrame::DataKeyFrame()
	: m_nValue(0)
{

}

DataKeyFrame::~DataKeyFrame()
{

}

//////////////////////////////////////////////////////////////////////////

ParallelTimeLine::ParallelTimeLine()
{
	m_pvpAnimation = new std::vector < Animation * > ;
}

ParallelTimeLine::~ParallelTimeLine()
{
	delete m_pvpAnimation;
	m_pvpAnimation = NULL;
}

//////////////////////////////////////////////////////////////////////////

Storyboard::Storyboard(Window *pParent)
	: m_pParentWnd(pParent)
	, m_dwRepeatTimes(1)
	, m_dwRepeatDuration(1000)
	, m_nAccelerationRatio(0)
	, m_nDecelerationRatio(0)
{
	m_pvpAnimation = new std::vector < Animation * > ;
}

Storyboard::~Storyboard()
{
	delete m_pvpAnimation;
	m_pvpAnimation = NULL;
}

int Storyboard::Begin()
{
	if (m_pvpAnimation->size() <= 0)
	{
		return 1;
	}

	std::vector<Animation *>::iterator it = m_pvpAnimation->begin();
	for (;  it != m_pvpAnimation->end(); ++it)
	{
		Animation *pAnimationItem = *it;

		if (pAnimationItem == NULL)
		{
			return -1;
		}

		ANIMATION_TARGET_PROPERTY eAtp = pAnimationItem->GetTargetProperty();
		(*m_pvpAnimation)[eAtp] = pAnimationItem;

		::SetTimer(m_pParentWnd->GetSafeHwnd(), IDT_STORYBOARD, 40, NULL);
	}

	return 0;
}

int Storyboard::Pause()
{
	return 0;
}

int Storyboard::Resume()
{
	return 0;
}

int Storyboard::Stop()
{
	return 0;
}

int Storyboard::SkipToFill()
{
	return 0;
}

int Storyboard::Seek(DWORD dwOffset)
{
	return 0;
}

int Storyboard::SetRepeatBehaviorTimes(DWORD nRepeat)
{
	m_dwRepeatTimes = nRepeat;
	return 0;
}

DWORD Storyboard::GetRepeatBehaviorTimes() const
{
	return m_dwRepeatTimes;
}

int Storyboard::SetRepeatBehaviorDuration(DWORD nRepeat)
{
	m_dwRepeatDuration = nRepeat;
	return 0;
}

DWORD Storyboard::GetRepeatBehaviorDuration() const
{
	return m_dwRepeatDuration;
}

int Storyboard::SetAccelerationRatio(int nRatio)
{
	m_nAccelerationRatio = nRatio;
	return 0;
}

int Storyboard::GetAccelerationRatio() const
{
	return m_nAccelerationRatio;
}

int Storyboard::SetDecelerationRatio(int nRatio)
{
	m_nDecelerationRatio = nRatio;
	return 0;
}

int Storyboard::GetDecelerationRatio() const
{
	return m_nDecelerationRatio;
}

int Storyboard::AddAnimation(Animation *pA)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

DataAnimation::DataAnimation()
	: m_nFrom(0)
	, m_nTo(0)
{
}

DataAnimation::~DataAnimation()
{
}

int DataAnimation::SetFrom(Gdiplus::REAL nFrom)
{
	m_bHasSetFrom = true;
	m_nFrom = nFrom;
	return 0;
}

Gdiplus::REAL DataAnimation::GetFrom() const
{
	return m_nFrom;
}

int DataAnimation::SetTo(Gdiplus::REAL nTo)
{
	m_bHasSetTo = true;
	m_nTo = nTo;
	return 0;
}

Gdiplus::REAL DataAnimation::GetTo() const
{
	return m_nTo;
}

//////////////////////////////////////////////////////////////////////////

DataAnimationUsingKeyFrames::DataAnimationUsingKeyFrames()
	: m_pvpKeyFrames(new std::vector<DataKeyFrame *>)
{

}

DataAnimationUsingKeyFrames::~DataAnimationUsingKeyFrames()
{
	delete m_pvpKeyFrames;
	m_pvpKeyFrames = NULL;
}

//////////////////////////////////////////////////////////////////////////

DataAnimationUsingPath::DataAnimationUsingPath()
	: m_nTarget(0)
	, m_ePathAnimationSource(PAS_X)
{

}

DataAnimationUsingPath::~DataAnimationUsingPath()
{

}

int DataAnimationUsingPath::SetDuration(DWORD dwDuration)
{
	return m_FlattenPath.SetDuration(dwDuration);
}

DWORD DataAnimationUsingPath::GetDuration() const
{
	return m_FlattenPath.GetDuration();
}

int DataAnimationUsingPath::AddPath(Gdiplus::GraphicsPath *pPath)
{
	if (pPath == NULL)
	{
		return -1;
	}

	return m_FlattenPath.AddPath(pPath);
}

int DataAnimationUsingPath::AddArcPath(Gdiplus::GraphicsPath *pPath,
	Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL width, Gdiplus::REAL height,
	Gdiplus::REAL startAngle, Gdiplus::REAL sweepAngle)
{
	if (pPath == NULL)
	{
		return -1;
	}

	return m_FlattenPath.AddArcPath(pPath, x, y, width, height, startAngle, sweepAngle);
}

int DataAnimationUsingPath::SetTargetValue(int nTarget)
{
	m_nTarget = nTarget;
	return 0;
}

int DataAnimationUsingPath::GetTargetValue() const
{
	return m_nTarget;
}

int DataAnimationUsingPath::GetPoint(DWORD t, __out CFlattenPathPoint *pPoint)
{
	return m_FlattenPath.GetPos(t, pPoint);
}

//////////////////////////////////////////////////////////////////////////

RectAnimation::RectAnimation()
{

}

RectAnimation::~RectAnimation()
{

}

int RectAnimation::SetFrom(LPCRECT lprcFrom)
{
	if (lprcFrom == NULL)
	{
		return -1;
	}

	m_rcFrom = lprcFrom;

	return 0;
}

int RectAnimation::GetFrom(LPRECT lprcFrom) const
{
	if (lprcFrom == NULL)
	{
		return -1;
	}

	*lprcFrom = m_rcFrom;

	return 0;
}

int RectAnimation::SetTo(LPCRECT lprcTo)
{
	if (lprcTo == NULL)
	{
		return -1;
	}

	m_rcTo = lprcTo;

	return 0;
}

int RectAnimation::GetTo(LPRECT lprcTo) const
{
	if (lprcTo == NULL)
	{
		return -1;
	}

	*lprcTo = m_rcTo;

	return 0;
}

//////////////////////////////////////////////////////////////////////////

