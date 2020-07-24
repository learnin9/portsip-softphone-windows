#include "stdafx.h"
#include "VoipCallBackEvent.h"
#include "SystemEx.h"
CVoipCallBackEvent::CVoipCallBackEvent()
{
}

CVoipCallBackEvent::~CVoipCallBackEvent()
{
}
void CVoipCallBackEvent::onMessage(void * command)
{
	if (!command)
	{
		return;
	}
	if (CSystemEx::GetInstance()->GetMainDlg()==NULL)
	{
		LOG4_INFO("Main Dlg Is Null,May Login The Main dlg is not init.process by Login Dlg");
		ICallbackParameters * pCallBackParameters = (ICallbackParameters *)command;
		SIP_EVENT et = pCallBackParameters->getEventType();
		CLoginDlg *pLoginDlg = CSystemEx::GetInstance()->GetCruuentLoginDlg();
		if (pLoginDlg==NULL)
		{
			LOG4_INFO("pLoginDlg is NULL ...OVER");
			return;
		}
		switch (pCallBackParameters->getEventType())
		{
		case SIP_REGISTER_SUCCESS:
			LOG4_INFO("Sip Register Success");
			pLoginDlg->ProcessLoginReslut(true);
			break;
		case SIP_REGISTER_FAILURE:
			LOG4_INFO("Sip Register Failure");
			std::string strErrorInfo = pCallBackParameters->getStatusText();
			LOG4_INFO(strErrorInfo.c_str());
			pLoginDlg->ProcessLoginReslut(false, strErrorInfo);
			break;
		}
		return;
	}
	
	CSystemEx::GetInstance()->GetMainDlg()->PostMessage(WM_SIPEVENT, (WPARAM)command, 0);
};