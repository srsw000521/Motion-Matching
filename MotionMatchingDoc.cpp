
// MotionMatchingDoc.cpp: CMotionMatchingDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MotionMatching.h"
#endif

#include "MotionMatchingDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMotionMatchingDoc

IMPLEMENT_DYNCREATE(CMotionMatchingDoc, CDocument)

BEGIN_MESSAGE_MAP(CMotionMatchingDoc, CDocument)
	ON_COMMAND(ID_MOTION_LOADBVHFILE, OnMotionLoadBVHFile)
	ON_COMMAND(ID_MOTIONMATCHING_DOEXPERIMENT, &CMotionMatchingDoc::OnMotionmatchingDoexperiment)
	ON_COMMAND(ID_MOTIONMATCHING_PRINTSKELETON, &CMotionMatchingDoc::OnMotionmatchingPrintskeleton)
	ON_COMMAND(ID_MOTIONMATCHING_PRINTCURRENTCOSTBOUND, &CMotionMatchingDoc::OnMotionmatchingPrintcurrentcostbound)
END_MESSAGE_MAP()


// CMotionMatchingDoc 생성/소멸

CMotionMatchingDoc::CMotionMatchingDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
}

CMotionMatchingDoc::~CMotionMatchingDoc()
{
}

void CMotionMatchingDoc::OnMotionLoadBVHFile()
{
	AfxMessageBox(_T("Load Motion!"));
}

BOOL CMotionMatchingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	MotionMatching = new CMotionMatching();
	trajectory = new Trajectory();
	MotionMatching->trajectory = trajectory;
	
	m_framenum = 0;
	m_currentTime = 0;
	m_lastTime = m_currentTime;

	//MotionMatching->testSetup();

	return TRUE;
}




// CMotionMatchingDoc serialization

void CMotionMatchingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CMotionMatchingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CMotionMatchingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMotionMatchingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMotionMatchingDoc 진단

#ifdef _DEBUG
void CMotionMatchingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMotionMatchingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMotionMatchingDoc 명령


void CMotionMatchingDoc::forwardFrame(int df)
{
	m_framenum += df;
	m_currentTime += df;
	
	trajectory->setCurrentTime(m_framenum);
	
	// mottion 추가  m_currentIndex ++ 
}

void CMotionMatchingDoc::applyMotionMatching()
{
	MotionMatching->applyMotionMatching();
	return;
}

void CMotionMatchingDoc::OnMotionmatchingDoexperiment()
{
	TCHAR strFilter[] = { _T("output data (*.txt)|*.txt|") };
	CFileDialog FileDlg(FALSE, CString(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(strFilter));

	if (FileDlg.DoModal() == IDCANCEL) return;

	CString path = FileDlg.GetPathName();
	CT2CA convertedString(path);
	std::string str = std::string(convertedString);
	std::cout << "Saving data to:" << str << std::endl;

	FILE* file;
	file = 	fopen(str.c_str(), "w");
	if (file == nullptr)
	{
		std::cout << "Error in writing file: " << str << std::endl;
		return;
	}

	MotionMatching->reset();
	
	MotionMatching->dstMotion.m_pSkeleton->root->setGlobalTransform();
	MotionMatching->dstMotion.m_pSkeleton->root->printGlobalPos();

	double height = MotionMatching->dstMotion.m_pSkeleton->getHeight();
	printf("err\n");
	for (int i = 0; i < trajectory->m_numSample; i++)
	{
		MotionMatching->applyMotionMatching();
		double err =  MotionMatching->computeErr();
		err /= height;
		err *= 1.8;
		forwardFrame();
		printf("frame %d \n", i);
		fprintf(file, "%lf\n", err);
	}

	fclose(file);
	std::cout << "Done!" << std::endl;

	UpdateAllViews(NULL);
}

void CMotionMatchingDoc::OnMotionmatchingPrintskeleton()
{
	MotionMatching->dstMotion.m_pSkeleton->root->setGlobalTransform();
	MotionMatching->dstMotion.m_pSkeleton->root->printGlobalPos();

	MotionMatching->dstMotion.m_pSkeleton->getHeight();
	std::cout << "height = " << MotionMatching->dstMotion.m_pSkeleton->m_height << std::endl;
}

void CMotionMatchingDoc::OnMotionmatchingPrintcurrentcostbound()
{
	MotionMatching->getCurrentCostBound();
}
