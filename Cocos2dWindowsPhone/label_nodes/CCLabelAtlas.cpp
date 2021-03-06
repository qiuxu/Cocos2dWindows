/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
* Copyright (c) 2010-2011 cocos2d-x.org
* Copyright (c) 2008-2010 Ricardo Quesada
* Copyright (c) 2011      Zynga Inc.
* 
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
* 
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
* 
* http://www.apache.org/licenses/LICENSE-2.0 
* 
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an 
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and limitations under the License.
*/

#include "pch.h"
#include "CCLabelAtlas.h"
#include "CCTextureAtlas.h"
#include "CCPointExtension.h"
#include "CCDrawingPrimitives.h"
#include "ccConfig.h"

namespace cocos2d{

	//CCLabelAtlas - Creation & Init
	CCLabelAtlas * CCLabelAtlas::create(const char *label, const char *charMapFile, unsigned int itemWidth, int unsigned itemHeight, unsigned char startCharMap)
	{
		CCLabelAtlas *pRet = new CCLabelAtlas();
		if(pRet && pRet->initWithString(label, charMapFile, itemWidth, itemHeight, startCharMap))
		{
			pRet->autorelease();
			return pRet;
		}
		CC_SAFE_DELETE(pRet)
		return NULL;
	}

	bool CCLabelAtlas::initWithString(const char *label, const char *charMapFile, unsigned int itemWidth, unsigned int itemHeight, unsigned char startCharMap)
	{
		CCAssert(label != NULL, "");
		if (CCAtlasNode::initWithTileFile(charMapFile, itemWidth, itemHeight, strlen(label)))
		{
			m_cMapStartChar = startCharMap;
			this->setString(label);
			return true;
		}
		return false;
	}
	CCLabelAtlas* CCLabelAtlas::create(const char *string, const char *fntFile)
{    
    CCLabelAtlas *ret = new CCLabelAtlas();
    if (ret)
    {
        if (ret->initWithString(string, fntFile))
        {
            ret->autorelease();
        }
        else 
        {
            CC_SAFE_RELEASE_NULL(ret);
        }
    }
    
    return ret;
}

bool CCLabelAtlas::initWithString(const char *theString, const char *fntFile)
{
  std::string pathStr = CCFileUtils::sharedFileUtils()->fullPathForFilename(fntFile);
  std::string relPathStr = pathStr.substr(0, pathStr.find_last_of("/"))+"/";
  CCDictionary *dict = CCDictionary::createWithContentsOfFile(pathStr.c_str());
  
  CCAssert(((CCString*)dict->objectForKey("version"))->intValue() == 1, "Unsupported version. Upgrade cocos2d version");
    
  std::string texturePathStr = relPathStr + ((CCString*)dict->objectForKey("textureFilename"))->getCString();
  CCString *textureFilename = CCString::create(texturePathStr);
  unsigned int width = ((CCString*)dict->objectForKey("itemWidth"))->intValue() / CC_CONTENT_SCALE_FACTOR();
  unsigned int height = ((CCString*)dict->objectForKey("itemHeight"))->intValue() / CC_CONTENT_SCALE_FACTOR();
  unsigned int startChar = ((CCString*)dict->objectForKey("firstChar"))->intValue();
  

  this->initWithString(theString, textureFilename->getCString(), width, height, startChar);
    
  return true;
}
	//CCLabelAtlas - Atlas generation
	void CCLabelAtlas::updateAtlasValues()
	{
		unsigned int n = m_sString.length();

		ccV3F_C4B_T2F_Quad quad;

		const unsigned char *s = (unsigned char*)m_sString.c_str();

        CCTexture2D *texture = m_pTextureAtlas->getTexture();
        float textureWide = (float) texture->getPixelsWide();
        float textureHigh = (float) texture->getPixelsHigh();

		for(unsigned int i = 0; i < n; i++) {
			unsigned char a = s[i] - m_cMapStartChar;
			float row = (float) (a % m_uItemsPerRow);
			float col = (float) (a / m_uItemsPerRow);

#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
            // Issue #938. Don't use texStepX & texStepY
            float left		= (2 * row * m_uItemWidth + 1) / (2 * textureWide);
            float right		= left + (m_uItemWidth * 2 - 2) / (2 * textureWide);
            float top		= (2 * col * m_uItemHeight + 1) / (2 * textureHigh);
            float bottom	= top + (m_uItemHeight * 2 - 2) / (2 * textureHigh);
#else
            float left		= row * m_uItemWidth / textureWide;
            float right		= left + m_uItemWidth / textureWide;
            float top		= col * m_uItemHeight / textureHigh;
            float bottom	= top + m_uItemHeight / textureHigh;
#endif // ! CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL

            quad.tl.texCoords.u = left;
            quad.tl.texCoords.v = top;
            quad.tr.texCoords.u = right;
            quad.tr.texCoords.v = top;
            quad.bl.texCoords.u = left;
            quad.bl.texCoords.v = bottom;
            quad.br.texCoords.u = right;
            quad.br.texCoords.v = bottom;

			quad.bl.vertices.x = (float) (i * m_uItemWidth);
			quad.bl.vertices.y = 0;
			quad.bl.vertices.z = 0.0f;
			quad.br.vertices.x = (float)(i * m_uItemWidth + m_uItemWidth);
			quad.br.vertices.y = 0;
			quad.br.vertices.z = 0.0f;
			quad.tl.vertices.x = (float)(i * m_uItemWidth);
			quad.tl.vertices.y = (float)(m_uItemHeight);
			quad.tl.vertices.z = 0.0f;
			quad.tr.vertices.x = (float)(i * m_uItemWidth + m_uItemWidth);
			quad.tr.vertices.y = (float)(m_uItemHeight);
			quad.tr.vertices.z = 0.0f;

			m_pTextureAtlas->updateQuad(&quad, i);
		}
	}
	
	//CCLabelAtlas - CCLabelProtocol
	void CCLabelAtlas::setString(const char *label)
	{
		unsigned int len = strlen(label);
		if (len > m_pTextureAtlas->getTotalQuads())
		{
			m_pTextureAtlas->resizeCapacity(len);
		}
		m_sString.clear();
		m_sString = label;
		this->updateAtlasValues();

		CCSize s;
		s.width = (float)(len * m_uItemWidth);
		s.height = (float)(m_uItemHeight);
		this->setContentSizeInPixels(s);

		m_uQuadsToDraw = len;
	}

	const char* CCLabelAtlas::getString(void)
	{
		return m_sString.c_str();
	}

	//CCLabelAtlas - draw

#if CC_LABELATLAS_DEBUG_DRAW	
	void CCLabelAtlas::draw()
	{
		CCAtlasNode::draw();

		const CCSize& s = this->getContentSize();
		CCPoint vertices[4]={
			ccp(0,0),ccp(s.width,0),
			ccp(s.width,s.height),ccp(0,s.height),
		};
		ccDrawPoly(vertices, 4, true);
	}
#endif
} // namespace cocos2d