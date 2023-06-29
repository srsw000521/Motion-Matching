
// MotionMatchingDoc.h: CMotionMatchingDoc 클래스의 인터페이스
//

#pragma once

#include "CMotionMatching.h"
#include "MySkeleton.h"
#include "Trajectory.h"



class CMotionMatchingDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMotionMatchingDoc() noexcept;
	DECLARE_DYNCREATE(CMotionMatchingDoc)

// 특성입니다.
public:
	//MySkeleton* skeleton;
	CMotionMatching* MotionMatching;
	Trajectory* trajectory;

	int m_framenum;// 작업입니다.
	int m_currentTime;
	int m_lastTime;


public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CMotionMatchingDoc();
	
#ifdef _DEBUG
	
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
	//void OnMotionLoadBVHFile();
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	void forwardFrame(int df = 1);
	void applyMotionMatching();
	void OnMotionLoadBVHFile();
	afx_msg void OnMotionmatchingDoexperiment();
	afx_msg void OnMotionmatchingPrintskeleton();
	afx_msg void OnMotionmatchingPrintcurrentcostbound();
};
