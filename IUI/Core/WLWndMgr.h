// CWLWnd管理类
// 例如管理CWLWnd的窗口区域，使CWLWnd的内容及子控件，无论如何绘制不到CWLWnd外面；
// 当使用CWLWnd::SetWindowRgn设置了裁剪区域后，CWLWnd的内容及子控件，还不能绘制到裁剪区域
// 的外面。 并且鼠标也要限定在这个区域内。 而这些工作，不应该是CWLWnd本身需要关心的，CWLWnd绘制
// 时，裁剪工作应该已经完成；CWLWnd命中测试时，也不应该考虑有SetWindowRgn这回事，在SetWindowRgn
// 外面，就不应该收到鼠标消息，而不是先收到鼠标消息后再判断是否在SetWindowRgn内部。

#pragma once


namespace CWLWndMgr
{
	// 设置CWLWnd为编辑模式。 当CWLWnd用于界面编辑器时，允许绘制CWLWnd外的子控件，也允许通过
	// 鼠标或键盘操作子控件。 这个机制应该去掉，因为作为CWLWnd，它无义务为Edit模式提供额外支持。
	// 一个功能完整的CWLWnd，用户应该通过它提供的接口，让它支持Edit模式需要的所有功能。
	// 比如：用户通过在上层处理Button的WM_CREATE消息，来禁止ComboBox的Button子控件创建
	// 而不是通过为ComboBox设置成Edit模式而禁止Button子控件创建。
	int SetEditMode(HWLWND hWnd, BOOL bEditMode);
	BOOL IsEditMode(HWLWND hWnd);


	// 在绘制CWLWnd前，要为CWLWnd设置裁剪区域，防止内容或子控件绘制到CWLWnd外。
	// 由于父子窗口都是绘制到同一个HDC上，在根控件绘制前，把HRGN设置成根控件一样大小
	// 在孩子绘制前，不能设置成孩子大小，因为孩子可能有部分或全部位置父控件之外。
	// 应该把父控件的HRGN、子控件的位置、子控件SetWindowRgn设置的HRGN组合起来，设置为子控件。
	// bClientArea：TRUE: 只绘制客户区，非客户区被裁掉。
	int SetClip(HWLWND hCtrl, HDC hDC, BOOL bClientArea);

} // namespace CWLWndMgr
