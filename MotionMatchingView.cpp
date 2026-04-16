
// MotionMatchingView.cpp: CMotionMatchingView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MotionMatching.h"
#endif

#include "MotionMatchingDoc.h"
#include "MotionMatchingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMotionMatchingView

IMPLEMENT_DYNCREATE(CMotionMatchingView, CView)

BEGIN_MESSAGE_MAP(CMotionMatchingView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_CONTROL_PLAY, &CMotionMatchingView::OnControlPlay)
	ON_COMMAND(ID_SHOW_SRC_MOTIOn, &CMotionMatchingView::OnShowSrcMotion)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SRC_MOTIOn, &CMotionMatchingView::OnUpdateShowSrcMotion)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_VIEW_SHOW_SRC1000FRAME, &CMotionMatchingView::OnViewShowSrc1000frame)
	ON_COMMAND(ID_SHOW_SRC_MOTION_ROOT_ONLY, &CMotionMatchingView::OnShowSrcMotionRootOnly)
	ON_COMMAND(ID_MOTIONMATCHING_CHANGETRAJECTORY, &CMotionMatchingView::OnMotionmatchingChangetrajectory)
	ON_COMMAND(ID_VIEW_SHOW_DST_MOTION, &CMotionMatchingView::OnViewShowDstMotion)
	ON_COMMAND(ID_SHOW_DST_MOTION_ROOT_ONLY, &CMotionMatchingView::OnShowDstMotionRootOnly)
END_MESSAGE_MAP()

// CMotionMatchingView 생성/소멸

CMotionMatchingView::CMotionMatchingView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_hRC = 0;
	m_pDC = 0;

	m_CX = 0;
	m_CY = 0;

	m_Aspect = 1.0f;

}

CMotionMatchingView::~CMotionMatchingView()
{
}

void CMotionMatchingView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	X_Angle = 0.0;
	Y_Angle = 0.0;
	m_Rotate = Quat(5.0f * 3.14 / 180, -5.0f * 3.14 / 180, 0.0f, 1.0f);
	m_Trans = Vector3f(0.0f, 0.0f, 0.0f);


	m_Size = 100.0f;


	m_Far = m_Size * 100.0f;
	m_Near = -m_Size * 100.01f;

	m_bRBut = false;
	m_bMBut = false;
	m_bLBut = false;

	m_bShowLightSource = true;
	m_bShowAxis = false;
	m_bPlay = false;

	m_bShowSrcMotion = false;
	m_bShowDstMotion = true;
	m_Show1000Frame = 0;
	m_bShowSrcMotionRootOnly = false;
	m_bShowDstMotionRootOnly = false;

	m_bShowDstMotion = false;

	m_dFrame = 0;
	m_trajectoryMode = 0;
}


BOOL CMotionMatchingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMotionMatchingView 그리기

void CMotionMatchingView::OnDraw(CDC* /*pDC*/)
{
	CMotionMatchingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	DrawScene();
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMotionMatchingView 진단

#ifdef _DEBUG
void CMotionMatchingView::AssertValid() const
{
	CView::AssertValid();
}

void CMotionMatchingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMotionMatchingDoc* CMotionMatchingView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMotionMatchingDoc)));
	return (CMotionMatchingDoc*)m_pDocument;
}
#endif //_DEBUG


// CMotionMatchingView 메시지 처리기


int CMotionMatchingView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pDC = new CClientDC(this);

	if (NULL == m_pDC)
	{
		::AfxMessageBox(_T("Can not get DC. \n"));
		return FALSE;
	}
	if (!SetupPixelFormat())
	{
		::AfxMessageBox(_T("SetupPixelFormat failed.\n"));
		return FALSE;
	}
	if (0 == (m_hRC = wglCreateContext(m_pDC->GetSafeHdc())))
	{
		::AfxMessageBox(_T("wglCreateContext failed.\n"));
		return FALSE;
	}
	if (FALSE == wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
	{
		::AfxMessageBox(_T("wglMakeCurrent failed.\n"));
		return FALSE;
	}

	SetTimer(0, 30, NULL);

	return 0;
}

BOOL CMotionMatchingView::SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER
		,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		8,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};
	int pixelformat;

	PIXELFORMATDESCRIPTOR* pPFDtoUse;

	pPFDtoUse = (0 == pPFD) ? &pfd : pPFD;

	if (0 == (pixelformat =
		::ChoosePixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse)))
	{
		::AfxMessageBox(_T("ChoosePixelFormat failed."));
		return FALSE;
	}

	if (FALSE == ::SetPixelFormat(m_pDC->GetSafeHdc(),
		pixelformat, pPFDtoUse))
	{
		::AfxMessageBox(_T("SetPixelFormat failed."));
		return FALSE;
	}
	return TRUE;
}

void CMotionMatchingView::OnDestroy()
{
	CView::OnDestroy();
	// TODO: Add your message handler code here
	if (FALSE == wglMakeCurrent(0, 0))
	{
		::AfxMessageBox(_T("wglMakeCurrent failed.\n"));
	}
	if (m_hRC && (FALSE == wglDeleteContext(m_hRC)))
	{
		::AfxMessageBox(_T("WglDeleteContext failed.\n"));
	}
	if (m_pDC)
		delete m_pDC;
}

BOOL CMotionMatchingView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
	
}

void CMotionMatchingView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	m_CX = cx;
	m_CY = cy;

	m_Aspect = (float)cx / (float)cy;

	/*
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glFrustum(-3.0,3.0,-3.0,3.0,1.0f,100.0f);
		//gluPerspective(45.0f,dAspect,0.1f,60.0f);
		glOrtho(-m_Size,m_Size,-m_Size/m_Aspect,m_Size/m_Aspect,m_Near, m_Far);
	*/
	glViewport(0, 0, cx, cy);
}

float t = 56 / 255.0f;
float bg_Color[] = { t, t, t };
void CMotionMatchingView::DrawBoard() {
	GLint x;
	GLint y;
	GLint colorCode = 1;

	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(-300, 0, -300);
	glRotatef(90, 1, 0, 0);
	for (y = 25; y <= 525; y += 25) {
		for (x = 25; x <= 525; x += 25) {
			if (colorCode == 1) {
				glColor3f(0.75, 0.75, 0.8);
				colorCode = 0;
			}
			else {
				glColor3f(.9, .9, 1.0);
				colorCode = 1;
			}
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			
			glVertex2i(x, y);
			glVertex2i(x, y + 25);
			glVertex2i(x + 25, y + 25);
			glVertex2i(x + 25, y);
			glEnd();
		}
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);

}

void CMotionMatchingView::DrawScene()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(bg_Color[0], bg_Color[1], bg_Color[2], 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawGrid();

	//	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (m_Size < 0.0f) m_Size = 0.0f;
	//	gluPerspective(45.0f*m_Size/5.0f,m_Aspect,m_Near,m_Far);
	glOrtho(-m_Size, m_Size, -m_Size / m_Aspect, m_Size / m_Aspect, m_Near, m_Far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -20.0f);

	DrawSettingLight();

	Vector3f rot = ln(m_Rotate);
	rot = rot / 3.141592f * 180.0f * 2.0f;
	float leng = sqrt(rot % rot);
	if (leng > 0.0001f)
	{
		rot = rot / leng;
		glRotated(leng, rot.x, rot.y, rot.z);
	}
	glTranslatef(m_Trans[0], m_Trans[1], m_Trans[2]);

	if (m_bShowAxis)	DrawAxis(1, m_Size / 5.0f);

	DrawBoard();

	DrawObjects();

	GLint viewport[4];
	GLfloat mvmatrix[16], projmatrix[16];
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projmatrix);

	//	DrawObjects2D();

	glFlush();
	SwapBuffers(m_pDC->m_hDC);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(projmatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(mvmatrix);

}

void CMotionMatchingView::DrawGrid(void)
{
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (m_Size < 0.0f) m_Size = 0.0f;

	glOrtho(-m_Size, m_Size, -m_Size / m_Aspect, m_Size / m_Aspect, m_Near, m_Far);
	//	glOrtho(0,m_CX,0,m_CY,m_Near, m_Far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glDisable(GL_LIGHTING);
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(75 / 255.0f, 75 / 255.0f, 75 / 255.0f);

	float tt = 1.0f;
	while (true)
	{
		if (tt > m_Size) break;
		tt *= 10.0f;
	}
	float step = tt / 10.0f;
	for (float x = -m_Size; x < m_Size; x += step)
	{
		glVertex2f(x, -m_Size / m_Aspect);
		glVertex2f(x, m_Size / m_Aspect);
	}
	for (float y = -m_Size / m_Aspect; y < m_Size / m_Aspect; y += step)
	{
		glVertex2f(-m_Size, y);
		glVertex2f(m_Size, y);
	}
	glEnd();

	glEnable(GL_DEPTH_TEST);
}

void CMotionMatchingView::DrawAxis(int mode, float scale)
{
	GLUquadricObj* q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glPushMatrix();

	if (mode == 0)
	{
		glTranslatef(-(10.0f - 1.0f), -(10.0f) / m_Aspect + 1.0f, 0.0f);
		Vector3f rot = ln(m_Rotate);
		rot = rot / 3.141592f * 180.0f * 2.0f;
		float leng = sqrt(rot % rot);
		if (leng > 0.0001f)
		{
			rot = rot / leng;
			glRotated(leng, rot.x, rot.y, rot.z);
		}
	}
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f * scale, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f * scale, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f * scale);
	glEnd();
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f * scale);
	gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
	glPopMatrix();

	

	glPushMatrix();

	glTranslatef(1.0f * scale, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 1.0f * scale, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
	glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(q);
}

void CMotionMatchingView::DrawSettingLight(void)
{
	GLfloat global_ambient[] = { 0.4f,0.4f,0.4f,1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glShadeModel(GL_SMOOTH);

	GLfloat ambientLight0[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat diffuseLight0[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat specularLight0[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat position0[] = { 300.0f,300.0f,300.0f,0.0f };				// 45 degree to the vertical

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);


	GLfloat ambientLight1[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat diffuseLight1[] = { 0.3f,0.3f,0.4f,1.0f };
	GLfloat specularLight1[] = { 0.3f,0.3f,0.4f,1.0f };
	GLfloat position1[] = { -300.0f,-300.0f,0.0f,0.0f };				// 45 degree to the vertical

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);


	glEnable(GL_FLAT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//	glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable(GL_COLOR_MATERIAL);

	//	GLfloat mspecular[] = {1.0f,1.0f,1.0f,1.0f};
	GLfloat mspecular[] = { 0.2f,0.2f,0.3f,1.0f };
	//	GLfloat mspecular[] = {0.0f,0.0f,0.0f,1.0f};
	GLfloat memission[] = { 0.0f,0.0f,0.0f,1.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mspecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, memission);


	glMateriali(GL_FRONT, GL_SHININESS, 50);


	if (m_bShowLightSource)
	{
		GLUquadricObj* sphere;
		sphere = gluNewQuadric();

		glDisable(GL_LIGHTING);

		glPushMatrix();
		glTranslatef(position0[0], position0[1], position0[2]);
		glColor4fv(diffuseLight0);
		gluSphere(sphere, 2.00f, 16, 16);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(position1[0], position1[1], position1[2]);
		glColor4fv(diffuseLight1);
		gluSphere(sphere, 2.00f, 16, 16);
		glPopMatrix();

		gluDeleteQuadric(sphere);
		glEnable(GL_LIGHTING);

	}
}

void CMotionMatchingView::DrawObjects(void)
{
	CMotionMatchingDoc* pDoc = GetDocument();

	pDoc->MotionMatching->draw(pDoc->m_framenum);

	pDoc->trajectory->drawTrajectory();

	pDoc->MotionMatching->draw(pDoc->m_currentTime + m_dFrame, m_bPlay);

	Vector3f goalPos = pDoc->trajectory->getCurrentGoalPosition();
	if (m_bShowDstMotion)
		pDoc->MotionMatching->dstMotion.drawMotionGL(-1,-1,-1, m_bShowSrcMotionRootOnly, m_bShowDstMotionRootOnly, goalPos);

	if (m_bShowSrcMotion)
	{
		if (m_Show1000Frame > pDoc->MotionMatching->srcMotion.numof_frame / 1000)
			m_Show1000Frame = 0;
		if (m_Show1000Frame == 0)
			pDoc->MotionMatching->srcMotion.drawMotionGL(-1,-1,-1, m_bShowSrcMotionRootOnly);
		else
			pDoc->MotionMatching->srcMotion.drawMotionGL(500, (m_Show1000Frame - 1) * 1000, m_Show1000Frame * 1000);

	}

	//if (m_bShowSrcMotion) pDoc->MotionMatching->srcMotion.drawMotionGL(100, 100);
	//if (m_bShowDstMotion) pDoc->MotionMatching->dstMotion.drawMotionGL(100);
	
}

void CMotionMatchingView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	{
		MouseDownPoint = point;
		m_bLBut = true;
		// capture mouse movements even outside window borders

		SetCapture();
		//	CView::OnLButtonDown(nFlags, point);
	}
	CView::OnLButtonDown(nFlags, point);
}

void CMotionMatchingView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_bLBut = false;
	MouseDownPoint = CPoint(0, 0);

	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
	Invalidate();
}


void CMotionMatchingView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseDownPoint = point;

	{

		m_bMBut = true;
		// capture mouse movements even outside window borders
		SetCapture();
		//	CView::OnLButtonDown(nFlags, point);
	}
	CView::OnMButtonDown(nFlags, point);
}


void CMotionMatchingView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseDownPoint = CPoint(0, 0);
	m_bMBut = false;
	// release mouse capture
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
	Invalidate();
}


void CMotionMatchingView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	{

		MouseDownPoint = point;

		m_bRBut = true;							// for Zooming in and out
		SetCapture();
	}
	CView::OnRButtonDown(nFlags, point);
}


void CMotionMatchingView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MouseDownPoint = CPoint(0, 0);
	m_bRBut = false;
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
	Invalidate();
}


void CMotionMatchingView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (GetCapture() == this)
	{
		if (m_bRBut == true)
		{
			m_Size += double(point.y - MouseDownPoint.y) / 10.0f * m_Size * 0.01f;

			Invalidate();
			MouseDownPoint = point;
		}
		else if (m_bMBut == true)
		{
			float px1, px2, py1, py2, pz1, pz2;
			double wx1, wx2, wy1, wy2, wz1, wz2;
			px1 = point.x;
			px2 = MouseDownPoint.x;
			py1 = point.y;
			py2 = MouseDownPoint.y;
			pz1 = 0.0f;
			pz2 = 0.0f;

			GLint viewport[4];
			GLdouble mvmatrix[16], projmatrix[16];

			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

			py1 = viewport[3] - (GLint)py1 - 1;
			py2 = viewport[3] - (GLint)py2 - 1;

			gluUnProject((GLdouble)px1, (GLdouble)py1, pz1,
				mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);
			gluUnProject((GLdouble)px2, (GLdouble)py2, pz2,
				mvmatrix, projmatrix, viewport, &wx2, &wy2, &wz2);

			m_Trans += Vector3f(wx1 - wx2, wy1 - wy2, wz1 - wz2);

			MouseDownPoint = point;
			Invalidate();
		}
		else if (m_bLBut)				/// L button
		{
			int ySize = 60;
			// increment the object rotation angles

			if (MouseDownPoint.y < ySize || m_CY - MouseDownPoint.y < ySize)
			{
				Vector3f vec_z;
				vec_z = Vector3f(0.0f, 0.0f, 1.0f) * float(point.x - MouseDownPoint.x) / 3.6f / 180.0f * 3.141592f / 2.0f;
				if (m_CY / 2 - MouseDownPoint.y > 0) vec_z = -vec_z;

				Quat rot = exp(vec_z);
				m_Rotate = rot * m_Rotate;
			}
			else
			{
				Vector3f vec_x, vec_y, vec_xy;
				vec_x = Vector3f(1.0f, 0.0f, 0.0f) * float(point.y - MouseDownPoint.y) / 1.0f / 180.0f * 3.141592f / 2.0f;
				vec_y = Vector3f(0.0f, 1.0f, 0.0f) * float(point.x - MouseDownPoint.x) / 1.0f / 180.0f * 3.141592f / 2.0f;
				vec_xy = (vec_x + vec_y) * 0.5f;
				Quat rot = exp(vec_xy);
				m_Rotate = rot * m_Rotate;
			}


			// redraw the view
			Invalidate();
			// remember the mouse point
			MouseDownPoint = point;
		}
	};
	CView::OnMouseMove(nFlags, point);
}


void CMotionMatchingView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bPlay == false) return;
	
	CMotionMatchingDoc* pDoc = GetDocument();
	if (nIDEvent == 0)
	{
		pDoc->forwardFrame();
		pDoc->applyMotionMatching();
		
		Invalidate();
	}
	CView::OnTimer(nIDEvent);
}


void CMotionMatchingView::OnControlPlay()
{
	m_bPlay = !m_bPlay;
	if(m_bPlay)
		printf("play!");
	else 
		printf("stop!");
}


void CMotionMatchingView::OnShowSrcMotion()
{
	m_bShowSrcMotion = !m_bShowSrcMotion;
	Invalidate();
}


void CMotionMatchingView::OnUpdateShowSrcMotion(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowSrcMotion);
}


void CMotionMatchingView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	
	CMotionMatchingDoc* pDoc = GetDocument();
	switch (nChar)
	{
	case VK_SPACE:
		m_bPlay = !m_bPlay;
		if (m_bPlay == true) break;

		m_dFrame = 0;
		pDoc->forwardFrame();
		pDoc->applyMotionMatching();
		Invalidate();
		break;
	case VK_LEFT:
		m_dFrame--;
		printf("drawing Frame = %d\n", pDoc->m_currentTime + m_dFrame);
		Invalidate();
		break;
	case VK_RIGHT:
		if (m_dFrame >= 0) break;
		m_dFrame++;
		printf("drawing Frame = %d\n", pDoc->m_currentTime + m_dFrame);
		Invalidate();
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMotionMatchingView::OnViewShowSrc1000frame()
{
	m_bShowSrcMotion = true;
	m_Show1000Frame++;
	Invalidate();
}


void CMotionMatchingView::OnShowSrcMotionRootOnly()
{
	m_bShowSrcMotionRootOnly = !m_bShowSrcMotionRootOnly;
	Invalidate();
}
void CMotionMatchingView::OnShowDstMotionRootOnly()
{
	m_bShowDstMotionRootOnly = !m_bShowDstMotionRootOnly;
	Invalidate();
}

void CMotionMatchingView::OnMotionmatchingChangetrajectory()
{
	m_trajectoryMode++;
	m_trajectoryMode %= 3;
	CMotionMatchingDoc* pDoc = GetDocument();
	//pDoc->MotionMatching->draw(pDoc->m_framenum);

	pDoc->trajectory->initTrajectory(m_trajectoryMode);

	Invalidate();
}


void CMotionMatchingView::OnViewShowDstMotion()
{
	m_bShowDstMotion = !m_bShowDstMotion;

	Invalidate();
}

// ---------------------------------------------------------------------------
// Render* helpers — GL logic extracted from data classes (Commit 1).
// DrawObjects still calls the old draw methods; these are not yet wired up.
// ---------------------------------------------------------------------------

void CMotionMatchingView::RenderJoint(MyJoint* joint, Vector3f color, int lineWidth)
{
	glPushMatrix();

	if (lineWidth < 1)
		glLineWidth(5.0f);
	else
		glLineWidth(lineWidth);

	glBegin(GL_LINES);

	Vector3f c = joint->color;
	if (color.x >= 0.0f)
		c = color;

	if (joint->parent != nullptr) {
		glColor3f(c.x, c.y, c.z);
		glVertex3f(0, 0, 0);
		glVertex3f(joint->localPos.x, joint->localPos.y, joint->localPos.z);
	}

	glEnd();

	glTranslatef(joint->localPos.x, joint->localPos.y, joint->localPos.z);

	Vector3f rot = ln(joint->localRot);
	rot = rot / 3.141592f * 180.0f * 2.0f;
	float leng = sqrt(rot % rot);
	if (leng > 0.0001f)
	{
		rot = rot / leng;
		glRotatef(leng, rot.x, rot.y, rot.z);
	}

	glPushMatrix();
	glPopMatrix();

	if (joint->child != nullptr)
		RenderJoint(joint->child, color, lineWidth);

	glPopMatrix();

	if (joint->sibling != nullptr)
		RenderJoint(joint->sibling, color, lineWidth);
}

void CMotionMatchingView::RenderJointShadow(MyJoint* joint, Vector3f color, int lineWidth)
{
	glPushMatrix();

	if (lineWidth < 1)
		glLineWidth(5.0f);
	else
		glLineWidth(lineWidth);

	if (joint->parent != nullptr)
	{
		glBegin(GL_LINES);
		glColor3f(color.x, color.y, color.z);
		glVertex3f(joint->shadowPos.x, joint->shadowPos.y, joint->shadowPos.z);
		glVertex3f(joint->parent->shadowPos.x, joint->parent->shadowPos.y, joint->parent->shadowPos.z);
		glEnd();
	}

	glPopMatrix();

	if (joint->child != nullptr)   RenderJointShadow(joint->child, color, lineWidth);
	if (joint->sibling != nullptr) RenderJointShadow(joint->sibling, color, lineWidth);
}

void CMotionMatchingView::RenderSkeleton(MySkeleton* skel, Vector3f color, int lineWidth,
                                          bool bSphere, bool srcOnly, bool dstOnly, float distance)
{
	if (!bSphere)
		glDisable(GL_LIGHTING);

	if (dstOnly)
	{
		skel->root->setGlobalTransform();
		Vector3f p = skel->root->globalPos;

		float height = skel->getHeight();
		float err = (height > 0.0f) ? (distance / height * 1.8f) : 0.0f;
		float h = 0.6f - err / 0.5f * 0.6f;
		if (h < 0.0f) h = 0.0f;

		skel->rootcolor = HSV2RGB(Vector3f(h, 1, 1));

		glColor3f(skel->rootcolor.x, skel->rootcolor.y, skel->rootcolor.z);
		glPushMatrix();
		glTranslatef(p.x, 0, p.z);
		GLUquadricObj* obj = gluNewQuadric();
		gluQuadricDrawStyle(obj, GLU_FILL);
		gluSphere(obj, 0.3f, 20, 20);
		gluDeleteQuadric(obj);
		glPopMatrix();
		return;
	}

	if (srcOnly)
	{
		skel->root->setGlobalTransform();
		Vector3f p = skel->root->globalPos;
		glColor3f(color.x, color.y, color.z);
		glPushMatrix();
		glTranslatef(p.x, 0, p.z);
		GLUquadricObj* obj = gluNewQuadric();
		gluQuadricDrawStyle(obj, GLU_FILL);
		gluSphere(obj, 0.3f, 20, 20);
		gluDeleteQuadric(obj);
		glPopMatrix();
		return;
	}

	RenderJoint(skel->root, color, lineWidth);
	skel->root->setGlobalTransform();
	RenderJointShadow(skel->root, Vector3f(0.3f, 0.3f, 0.3f), lineWidth);

	if (bSphere)
	{
		for (int i = 0; i < skel->numJoints; i++)
		{
			Vector3f p = skel->joints[i]->globalPos;
			glPushMatrix();
			glTranslatef(p.x, p.y, p.z);
			GLUquadricObj* obj = gluNewQuadric();
			gluQuadricDrawStyle(obj, GLU_FILL);
			glColor3f(skel->joints[i]->color.x, skel->joints[i]->color.y, skel->joints[i]->color.z);
			gluSphere(obj, 0.3f, 20, 20);
			gluDeleteQuadric(obj);
			glPopMatrix();
		}
	}
}

void CMotionMatchingView::RenderPosture(MyPosture* posture, MySkeleton* skel, Vector3f color,
                                         bool bSphere, bool srcOnly, bool dstOnly)
{
	skel->setPosture(*posture);
	RenderSkeleton(skel, color, 1, bSphere, srcOnly, dstOnly, posture->m_distance);
}

void CMotionMatchingView::RenderMotion(Motion* motion, int numFrames, int stFrame, int edFrame,
                                        bool srcOnly, bool dstOnly)
{
	if (motion->m_pSkeleton == nullptr) return;
	if (motion->postures.size() == 0) return;

	if (stFrame < 1) stFrame = 0;
	if (edFrame < 1) edFrame = (int)motion->postures.size();
	if (stFrame > edFrame) stFrame = edFrame;

	if (numFrames < 1) numFrames = edFrame;
	int step = (edFrame - stFrame) / numFrames;
	if (step < 1) step = 1;

	for (int i = stFrame; i < edFrame; i += step)
	{
		MyPosture& p = motion->postures[i];
		Vector3f c = Vector3f(0, float(i) / motion->postures.size(), 1 - float(i) / motion->postures.size());
		RenderPosture(&p, motion->m_pSkeleton, c, false, srcOnly, dstOnly);
	}
}

void CMotionMatchingView::RenderTrajectoryPath(Trajectory* traj)
{
	glLineWidth(10.0f);
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < traj->m_numSample; i++)
		glVertex3f(traj->m_trajectory[i].x, traj->m_trajectory[i].y, traj->m_trajectory[i].z);
	glEnd();
}

void CMotionMatchingView::RenderTrajectoryCurrentGoal(Trajectory* traj)
{
	Vector3f p = traj->getPositionAt(traj->m_goalIndex);
	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	GLUquadricObj* obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	glColor3f(1, 0, 0);
	gluSphere(obj, 0.7f, 20, 20);
	gluDeleteQuadric(obj);
	glPopMatrix();
}

void CMotionMatchingView::RenderTrajectoryFutureGoal(Trajectory* traj, int dframe)
{
	Vector3f p = traj->getFuturePosition(dframe);
	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	GLUquadricObj* obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	glColor3f(1, 0, 0);
	gluSphere(obj, 0.5f, 20, 20);
	gluDeleteQuadric(obj);
	glPopMatrix();
}

void CMotionMatchingView::RenderFuturePositions(CMotionMatching* mm)
{
	if (mm->dstMotion.postures.size() == 0) return;

	GLUquadricObj* obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);

	MyPosture& p = mm->dstMotion.postures.back();
	Quat q = p.rootOriY;

	for (int i = 0; i < mm->srcMotion.m_numStep; i++)
	{
		Vector3f pos = p.rootPosition + rotate(q, mm->srcMotion.m_features[mm->m_currentIndex].m_posFuture[i]);
		glPushMatrix();
		glTranslatef(pos.x, 0, pos.z);
		glColor3f(0, 1, 0);
		gluSphere(obj, 0.5f, 20, 20);
		glPopMatrix();
	}

	gluDeleteQuadric(obj);
}

