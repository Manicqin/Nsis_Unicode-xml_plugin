/*****************************************************************
 *                    XML NSIS plugin v2.0                       *
 *                                                               *
 * 2008 Shengalts Aleksander aka Instructor (Shengalts@mail.ru)  *
 *****************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include "StackFunc.h"
#include "tinyxml.h"
#include "xpath_static.h"

#ifdef UNICODE
#include "nsis_unicode\pluginapi.h"
#else
#include "nsis_ansi\pluginapi.h"
#endif
#include <assert.h>
/* Defines */
#define NSIS_MAX_STRLEN 1024

/* Include private stack functions */

#include "StackFunc.h"
#include <string>


typedef struct _xml_stack {
  struct _xml_stack *next;
  struct _xml_stack *prev;
  TiXmlNode *st_node;
} xml_stack;

#define NSISFUNC(name) extern "C" void __declspec(dllexport) name(HWND hWndParent, int string_size, TCHAR* variables, stack_t** stacktop, extra_parameters* extra)
#define DLL_INIT() EXDLL_INIT(); extra->RegisterPluginCallback((HMODULE)g_hInstance, PluginCallback)

/* Global variables */
TCHAR szBuf[NSIS_MAX_STRLEN];
TCHAR szBuf2[NSIS_MAX_STRLEN];
int nGoto=0;
int nElement=1;
TiXmlDocument doc;
TiXmlNode *node=&doc;
TiXmlNode *nodeF=0;
TiXmlNode *nodeF2=0;
TiXmlNode *nodeTmp=0;
TiXmlNode *nodeTmp2=0;
TiXmlText *text=0;
TiXmlElement *element=0;
TiXmlAttribute *attribute=0;
TiXmlAttribute *attributeTmp=0;
TiXmlDeclaration *declaration=0;
TiXmlEncoding TIXML_ENCODING=TIXML_DEFAULT_ENCODING;
xml_stack *pStackElement=NULL;
xml_stack *pStackFirst=NULL;
xml_stack *pStackTop=NULL;


/* NSIS functions code */
NSISFUNC(_SetCondenseWhiteSpace)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (!lstrcmp(variables, TEXT("0")))
      doc.SetCondenseWhiteSpace(false);
    else
      doc.SetCondenseWhiteSpace(true);
  }
}

NSISFUNC (_SetEncoding)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (!lstrcmpi(variables, TEXT("UTF8"))) TIXML_ENCODING=TIXML_ENCODING_UTF8;
    else if (!lstrcmpi(variables, TEXT("LEGACY"))) TIXML_ENCODING=TIXML_ENCODING_LEGACY;
  }
}

NSISFUNC (_LoadFile)
{
  EXDLL_INIT();
  {
	assert(false);
    popstring(szBuf);

	
    if (doc.LoadFile(ws2s(szBuf), TIXML_ENCODING))
      pushstring(TEXT("0"));
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_SaveFile)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && doc.SaveFile()) || (*szBuf && doc.SaveFile(ws2s(szBuf))))
      pushstring(TEXT("0"));
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_DeclarationVersion)
{
  EXDLL_INIT();
  {
    nodeTmp=&doc;

    if ((nodeTmp=nodeTmp->FirstChild()) && (declaration=nodeTmp->ToDeclaration()))
    {
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(declaration->Version()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_DeclarationEncoding)
{
  EXDLL_INIT();
  {
    nodeTmp=&doc;

    if ((nodeTmp=nodeTmp->FirstChild()) && (declaration=nodeTmp->ToDeclaration()))
    {
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(declaration->Encoding()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_DeclarationStandalone)
{
  EXDLL_INIT();
  {
    nodeTmp=&doc;

    if ((nodeTmp=nodeTmp->FirstChild()) && (declaration=nodeTmp->ToDeclaration()))
    {
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(declaration->Standalone()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_GetText)
{
  EXDLL_INIT();
  {
    if (node->ToElement())
    {
      if ((nodeTmp=node->FirstChild()) && nodeTmp->ToText())
      {
        pushstring(TEXT("0"));
        pushstring(cnvrToWide(nodeTmp->Value()));
        return;
      }
    }
    pushstring(TEXT("-1"));
    pushstring(TEXT(""));
  }
}

NSISFUNC (_SetText)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (node->ToElement())
    {
       if (nodeTmp=node->FirstChild())
      {
        if (nodeTmp->ToText())
          nodeTmp->SetValue(ws2s(szBuf));
        else
        {
          text=new TiXmlText(ws2s(szBuf));
          node->InsertBeforeChild(nodeTmp, *text);
        }
      }
      else
      {
        text=new TiXmlText(ws2s(szBuf));
        node->InsertEndChild(*text);
      }
      pushstring(TEXT("0"));
      return;
    }
    pushstring(TEXT("-1"));
  }
}

NSISFUNC (_SetCDATA)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (text=node->ToText())
    {
      if (!lstrcmp(szBuf, TEXT("1")))
        text->SetCDATA(true);
      else
        text->SetCDATA(false);
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_IsCDATA)
{
  EXDLL_INIT();
  {
    if (text=node->ToText())
    {
      if (text->CDATA() == TRUE)
        pushstring(TEXT("1"));
      else
        pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_GetNodeValue)
{
  EXDLL_INIT();
  {
    pushstring(cnvrToWide(node->Value()));
  }
}

NSISFUNC (_SetNodeValue)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    node->SetValue(ws2s(szBuf));
  }
}

//NSISFUNC (_FindNextElement)
//{
//  EXDLL_INIT();
//  {
//    int i;
//
//    popstring(szBuf);
//
//    if (nGoto == 1) goto NextSiblingElement;
//    else if (nGoto != 0) goto End;
//
//    nGoto=1;
//    StackInsert((stack **)&pStackFirst, (stack **)&pStackTop, (stack **)&pStackElement, -1, sizeof(xml_stack));
//    pStackElement->st_node=node;
//
//    while (nElement != 0)
//    {
//      --nElement;
//      nodeF=pStackTop->st_node;
//      StackDelete((stack **)&pStackFirst, (stack **)&pStackTop, (stack *)pStackTop);
//
//      if ((!*szBuf && (nodeF2=nodeF->FirstChildElement())) || (*szBuf && (nodeF2=nodeF->FirstChildElement(ws2s(szBuf)))))
//      {
//        Return:
//        node=nodeF2;
//        pushstring(TEXT("0"));
//        pushstring(cnvrToWide(node->Value()));
//        return;
//
//        NextSiblingElement:
//        if ((!*szBuf && (nodeF2=nodeF2->NextSiblingElement())) || (*szBuf && (nodeF2=nodeF2->NextSiblingElement(ws2s(szBuf)))))
//          goto Return;
//      }
//      if (nodeF2=nodeF->FirstChildElement())
//      {
//        i=0;
//
//        do
//        {
//          ++i;
//          ++nElement;
//          StackInsert((stack **)&pStackFirst, (stack **)&pStackTop, (stack **)&pStackElement, i, sizeof(xml_stack));
//          pStackElement->st_node=nodeF2;
//        }
//        while (nodeF2=nodeF2->NextSiblingElement());
//      }
//    }
//    nGoto=-1;
//
//    End:
//    pushstring(TEXT("-1"));
//    pushstring(TEXT(""));
//  }
//}

//NSISFUNC (_FindCloseElement)
//{
//  nGoto=0;
//  StackClear((stack **)&pStackFirst, (stack **)&pStackTop);
//}

NSISFUNC (_RootElement)
{
  EXDLL_INIT();
  {
    if (nodeTmp=doc.RootElement())
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_FirstChildElement)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->FirstChildElement())) || (*szBuf && (nodeTmp=node->FirstChildElement(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_FirstChild)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->FirstChild())) || (*szBuf && (nodeTmp=node->FirstChild(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
   
	}
  }
}
NSISFUNC (_LastChild)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->LastChild())) || (*szBuf && (nodeTmp=node->LastChild(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_Parent)
{
  EXDLL_INIT();
  {
    if (nodeTmp=node->Parent())
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_NoChildren)
{
  EXDLL_INIT();
  {
    if (node->NoChildren() == TRUE)
      pushstring(TEXT("1"));
    else
      pushstring(TEXT("0"));
  }
}

NSISFUNC (_NextSiblingElement)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->NextSiblingElement())) || (*szBuf && (nodeTmp=node->NextSiblingElement(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_NextSibling)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->NextSibling())) || (*szBuf && (nodeTmp=node->NextSibling(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_PreviousSibling)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((!*szBuf && (nodeTmp=node->PreviousSibling())) || (*szBuf && (nodeTmp=node->PreviousSibling(ws2s(szBuf)))))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(node->Value()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_InsertAfterNode)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((nodeTmp2=(TiXmlNode *)atoi(ws2s(szBuf))) && (nodeTmp=node->Parent()) && (nodeTmp=nodeTmp->InsertAfterChild(node, *nodeTmp2)))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_InsertBeforeNode)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((nodeTmp2=(TiXmlNode *)atoi(ws2s(szBuf))) && (nodeTmp=node->Parent()) && (nodeTmp=nodeTmp->InsertBeforeChild(node, *nodeTmp2)))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_InsertEndChild)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (node->ToElement() && (nodeTmp2=(TiXmlNode *)atoi(ws2s(szBuf))) && (nodeTmp=node->InsertEndChild(*nodeTmp2)))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_ReplaceNode)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((nodeTmp2=(TiXmlNode *)atoi(ws2s(szBuf))) && (nodeTmp=node->Parent()) && (nodeTmp->ReplaceChild(node, *nodeTmp2)))
      pushstring(TEXT("0"));
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_RemoveNode)
{
  EXDLL_INIT();
  {
    if ((nodeTmp=node->Parent()) && nodeTmp->RemoveChild(node))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_RemoveAllChild)
{
  EXDLL_INIT();
  {
    node->Clear();
  }
}

NSISFUNC (_CreateText)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (text=new TiXmlText(ws2s(szBuf)))
    {
		char	tmp[256];
      _itoa((int)text, tmp, 10);
      pushstring(cnvrToWide(tmp));
    }
    else
      pushstring(TEXT("0"));
  }
}

NSISFUNC (_CreateNode)
{
  EXDLL_INIT();
  {
    TiXmlElement element("");

    popstring(szBuf);

    if (element.Parse(ws2s(szBuf), 0, TIXML_ENCODING))
    {
      _itot((int)element.Clone(), szBuf, 10);
      pushstring(szBuf);
    }
    else
      pushstring(TEXT("0"));
  }
}

NSISFUNC (_CloneNode)
{
  EXDLL_INIT();
  {
    _itot((int)node->Clone(), szBuf, 10);

    pushstring(szBuf);
  }
}

NSISFUNC (_FreeNode)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (nodeTmp=(TiXmlNode *)atoi(ws2s(szBuf)))
    {
      nodeTmp->~TiXmlNode();
      nodeTmp=0;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_NodeHandle)
{
  EXDLL_INIT();
  {
    _itot((int)node, szBuf, 10);

    pushstring(szBuf);
  }
}

NSISFUNC (_GotoHandle)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (nodeTmp=(TiXmlNode *)atoi(ws2s(szBuf)))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_ElementPath)
{
  EXDLL_INIT();
  {
    int i;
    szBuf[0]='\0';
    nodeTmp=node;

    if (nodeTmp != &doc && !nodeTmp->ToElement())
      nodeTmp=nodeTmp->Parent();

    if (nodeTmp != &doc)
    {
      do
      {
        nodeTmp2=nodeTmp;
        i=1;

        while (nodeTmp2=nodeTmp2->PreviousSibling(nodeTmp->Value()))
          if (nodeTmp2->ToElement()) ++i;

        if (i != 1)
          _stprintf(szBuf2, _TEXT("/%s[%d]%s"), s2ws(nodeTmp->Value()), i, szBuf);
        else
          _stprintf(szBuf2, _TEXT("/%s%s"), s2ws(nodeTmp->Value()), szBuf);

        lstrcpy(szBuf, szBuf2);
      }
      while ((nodeTmp=nodeTmp->Parent()) && nodeTmp != &doc);
    }
    pushstring(szBuf);
  }
}

NSISFUNC (_GotoPath)
{
  EXDLL_INIT();
  {
    auto *pBuf=&szBuf[0];
    auto *pBuf2=&szBuf[0];
    int i;
    BOOL bExit=FALSE;
    nodeTmp=node;

    popstring(szBuf);

    if (*pBuf2 == _T('\0'))
    {
      nodeTmp=&doc;
      bExit=TRUE;
    }
    else if (*pBuf2 == _T('/'))
      nodeTmp=&doc;
    else if (nodeTmp != &doc && !nodeTmp->ToElement())
      goto Error;
    else --pBuf2;

    while (nodeTmp && bExit == FALSE)
    {
      i=1;
      pBuf=++pBuf2;

      while (*pBuf2 != _T('/') && *pBuf2 != _T('\0'))
      {
        if (*pBuf2 == _T('['))
        {
          *pBuf2=_T('\0');
		  ++pBuf2;
          i=atoi(ws2s(pBuf2));


          if (i == 0) goto Error;
        }
        else ++pBuf2;
      }

      if (*pBuf2 == _T('/')) *pBuf2=_T('\0');
      else bExit=TRUE;

      if (i > 0)
      {
        if ((!*pBuf && (nodeTmp=nodeTmp->FirstChildElement())) || (*pBuf && (nodeTmp=nodeTmp->FirstChildElement(ws2s(pBuf)))))
        {
          do
          {
            --i;
          }
          while (i != 0 && ((!*pBuf && (nodeTmp=nodeTmp->NextSiblingElement())) || (*pBuf && (nodeTmp=nodeTmp->NextSiblingElement(ws2s(pBuf))))));
        }
      }
      else
      {
        if ((!*pBuf && (nodeTmp=nodeTmp->LastChild())) || (*pBuf && (nodeTmp=nodeTmp->LastChild(ws2s(pBuf)))))
        {
          do
          {
            if (nodeTmp->ToElement()) ++i;
          }
          while (i != 0 && ((!*pBuf && (nodeTmp=nodeTmp->PreviousSibling())) || (*pBuf && (nodeTmp=nodeTmp->PreviousSibling(ws2s(pBuf))))));
        }
      }
    }

    if (nodeTmp && bExit == TRUE)
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
      return;
    }

    Error:
    pushstring(TEXT("-1"));
  }
}

NSISFUNC (_XPathString)
{
  EXDLL_INIT();
  {
    TIXML_STRING S_res;

    popstring(szBuf);

    if (TinyXPath::o_xpath_string(node, ws2s(szBuf), S_res))
    {
      pushstring(TEXT("0"));
      pushstring(s2ws(S_res.c_str()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_XPathNode)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (TinyXPath::o_xpath_node(node, ws2s(szBuf), nodeTmp))
    {
      node=nodeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_XPathAttribute)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (TinyXPath::o_xpath_attribute(node, ws2s(szBuf), attributeTmp))
    {
      attribute=attributeTmp;
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_NodeType)
{
  EXDLL_INIT();
  {
    int nType;

    nType=node->Type();
    if (nType == 1) pushstring(TEXT("ELEMENT"));
    else if (nType == 2) pushstring(TEXT("COMMENT"));
    else if (nType == 3) pushstring(TEXT("DOCUMENT"));
    else if (nType == 4) pushstring(TEXT("TEXT"));
    else if (nType == 5) pushstring(TEXT("DECLARATION"));
    else pushstring(TEXT("UNKNOWN"));
  }
}

NSISFUNC (_Coordinate)
{
  EXDLL_INIT();
  {
    _itot(node->Row(), szBuf, 10);
    _itot(node->Column(), szBuf2, 10);

    pushstring(szBuf2);
    pushstring(szBuf);
  }
}

NSISFUNC (_GetAttribute)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if ((element=node->ToElement()) && (attributeTmp=element->FirstAttribute()))
    {
      do
      {
        if (!lstrcmp(szBuf, s2ws(attributeTmp->Name())))
        {
          attribute=attributeTmp;
          pushstring(TEXT("0"));
          pushstring(cnvrToWide(attribute->Value()));
          return;
        }
      }
      while (attributeTmp=attributeTmp->Next());
    }
    pushstring(TEXT("-1"));
    pushstring(TEXT(""));
  }
}

NSISFUNC (_SetAttribute)
{
  EXDLL_INIT();
  {
    popstring(szBuf);
    popstring(szBuf2);

    if (element=node->ToElement())
    {
      element->SetAttribute(ws2s(szBuf), ws2s(szBuf2));
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_RemoveAttribute)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    if (element=node->ToElement())
    {
      element->RemoveAttribute(ws2s(szBuf));
      pushstring(TEXT("0"));
    }
    else
      pushstring(TEXT("-1"));
  }
}

NSISFUNC (_FirstAttribute)
{
  EXDLL_INIT();
  {
    if ((element=node->ToElement()) && (attributeTmp=element->FirstAttribute()))
    {
      attribute=attributeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(attribute->Value()));
      pushstring(cnvrToWide(attribute->Name()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_LastAttribute)
{
  EXDLL_INIT();
  {
    if ((element=node->ToElement()) && (attributeTmp=element->LastAttribute()))
    {
      attribute=attributeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(attribute->Value()));
      pushstring(cnvrToWide(attribute->Name()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_NextAttribute)
{
  EXDLL_INIT();
  {
    if (attribute && (attributeTmp=attribute->Next()))
    {
      attribute=attributeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(attribute->Value()));
      pushstring(cnvrToWide(attribute->Name()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_PreviousAttribute)
{
  EXDLL_INIT();
  {
    if (attribute && (attributeTmp=attribute->Previous()))
    {
      attribute=attributeTmp;
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(attribute->Value()));
      pushstring(cnvrToWide(attribute->Name()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_CurrentAttribute)
{
  EXDLL_INIT();
  {
    if (attribute)
    {
      pushstring(TEXT("0"));
      pushstring(cnvrToWide(attribute->Value()));
      pushstring(cnvrToWide(attribute->Name()));
    }
    else
    {
      pushstring(TEXT("-1"));
      pushstring(TEXT(""));
      pushstring(TEXT(""));
    }
  }
}

NSISFUNC (_SetAttributeName)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    attribute->SetName(ws2s(szBuf));
  }
}

NSISFUNC (_SetAttributeValue)
{
  EXDLL_INIT();
  {
    popstring(szBuf);

    attribute->SetValue(ws2s(szBuf));
  }
}

NSISFUNC (_Unload)
{
}

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  return TRUE;
}


