// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("LD")

AcDbObjectId CreateLine() {	//创建一条直线
	AcGePoint3d StartPoint(0.0, 0.0, 0.0);	//创建三维起点
	AcGePoint3d EndPoint(10.0, 10.0, 0.0);	//创建三维终点
	AcDbLine* pLine = new AcDbLine(StartPoint, EndPoint);	//创建直线

	AcDbBlockTable* pBlkTbl;	//创建块表
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl,AcDb::kForRead);	//读取块表
	AcDbBlockTableRecord* pBlkRcd;	//创建记录
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkRcd, AcDb::kForWrite);	//直线写入记录
	pBlkTbl->close();	//关闭指针

	AcDbObjectId LineId;	//创建Id
	pBlkRcd->appendAcDbEntity(LineId, pLine);	//
	pLine->close();
	pBlkRcd->close();

	return LineId;
}

AcDbObjectId CreateCircle() {	//创建一个圆
	AcGePoint3d center(9.0, 9.0, 0.0);
	AcGeVector3d normal(0.0, 0.0, 1.0);
	AcDbCircle* pCircle = new AcDbCircle(center, normal, 10.0);

	AcDbBlockTable* pBlkTbl;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkRcd;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkRcd, AcDb::kForWrite);
	pBlkTbl->close();

	AcDbObjectId CircleId;
	pBlkRcd->appendAcDbEntity(CircleId, pCircle);
	pCircle->close();
	pBlkRcd->close();

	return CircleId;
}

void CreateLayer() {	//创建图层
	AcDbLayerTable* pLayTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayTbl, AcDb::kForWrite);
	AcDbLayerTableRecord* pLayRcd = new AcDbLayerTableRecord;
	pLayRcd->setName(_T("newLayer"));
	pLayTbl->add(pLayRcd);
	pLayRcd->close();
	pLayTbl->close();

	return;
}

void CreateGroup(AcDbObjectIdArray ObjId, ACHAR* GroupName) {	//创建组
	AcDbGroup* pGroup = new AcDbGroup(GroupName, true);
	for (int i = 0; i < ObjId.length(); i++) {
		pGroup->append(ObjId.at(i));
	}

	AcDbDictionary* pDict;
	acdbHostApplicationServices()->workingDatabase()->getGroupDictionary(pDict, AcDb::kForWrite);
	AcDbObjectId groupId;
	pDict->setAt(GroupName, pGroup, groupId);
	pDict->close();
	pGroup->close();

	return;
}

void ChangeColor(AcDbObjectId objid, Adesk::Int16 color) {	//设置实体颜色
	AcDbEntity* pEnt;
	acdbOpenObject(pEnt, objid, AcDb::kForWrite);
	pEnt->setColorIndex(color);
	pEnt->close();
}

void DoEnt() {	//执行函数
	AcDbObjectId LineId = CreateLine();
	AcDbObjectId CircleId = CreateCircle();
	
	CreateLayer();
	AcDbObjectIdArray objids;
	objids.append(LineId);
	objids.append(CircleId);

	ChangeColor(LineId, 0);
	ChangeColor(CircleId, 64);

	CreateGroup(objids, _T("MyGroupLD"));

	

	return;
}



//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxProject2App : public AcRxArxApp {

public:
	CArxProject2App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		acedRegCmds->addCommand(_T("LDCmds"), _T("DoEnt"), _T("kaishi"), ACRX_CMD_MODAL, DoEnt);

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here
		acedRegCmds->removeGroup(_T("LDCmds"));

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CArxProject2App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	static void LDMyGroupMyCommand () {
		// Put your command code here
		acdbHostApplicationServices()->workingDatabase();
	}

	// Modal Command with pickfirst selection
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET)
	static void LDMyGroupMyPickFirst () {
		ads_name result ;
		int iRet =acedSSGet (ACRX_T("_I"), NULL, NULL, NULL, result) ;
		if ( iRet == RTNORM )
		{
			// There are selected entities
			// Put your command using pickfirst set code here
		}
		else
		{
			// There are no selected entities
			// Put your command code here
		}
	}

	// Application Session Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION)
	static void LDMyGroupMySessionCmd () {
		// Put your command code here
	}

	// The ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO macros can be applied to any static member 
	// function of the CArxProject2App class.
	// The function may or may not take arguments and have to return RTNORM, RTERROR, RTCAN, RTFAIL, RTREJ to AutoCAD, but use
	// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal to return
	// a value to the Lisp interpreter.
	//
	// NOTE: ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid.
	
	//- ACED_ADSFUNCTION_ENTRY_AUTO(classname, name, regFunc) - this example
	//- ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//- ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) - a Lisp command (prefix C:)
	//- ACED_ADSCOMMAND_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file

	// Lisp Function is similar to ARX Command but it creates a lisp 
	// callable function. Many return types are supported not just string
	// or integer.
	// ACED_ADSFUNCTION_ENTRY_AUTO(CArxProject2App, MyLispFunction, false)
	static int ads_MyLispFunction () {
		//struct resbuf *args =acedGetArgs () ;
		
		// Put your command code here

		//acutRelRb (args) ;
		
		// Return a value to the AutoCAD Lisp Interpreter
		// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal

		return (RTNORM) ;
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject2App)

ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject2App, LDMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
ACED_ADSSYMBOL_ENTRY_AUTO(CArxProject2App, MyLispFunction, false)

