#pragma once

#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <learnopengl/shader.h>

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

typedef std::map<char, Character> GlyphMap;

class Font
{
public:

	inline static unsigned int DEFAULT_FONT_SIZES[] = {
		16, 18, 20, 24, 28, 32, 48, 64
	};

	Font(const std::string& path)
		: m_path(path)
	{
	}

	bool HasSize(unsigned int size)
	{
		return m_glyphs.find(size) != m_glyphs.end();
	}

	std::map<char, Character>& GetCharacters(unsigned int size)
	{
		return m_glyphs[size];
	}

	void AddGlypMap(unsigned int size, const GlyphMap& glyphMap)
	{
		m_glyphs.insert({ size, glyphMap });
	}

    std::string GetPath() const
    {
        return m_path;
    }

protected:
    std::string m_path;
	std::map<unsigned int, GlyphMap> m_glyphs;
};

class FontSystem
{
public:
    virtual ~FontSystem() = default;
    //virtual int Initialize() = 0;
    virtual void RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color) = 0;

    virtual void LoadFont(Font& font) = 0;

    void SetFont(const std::string& name)
    {
        if (m_fontMap.find(name) != m_fontMap.end())
            m_currentFont = m_fontMap[name];
    }

protected:
    inline static std::unique_ptr<Font> s_defaultFont = nullptr;

    std::unordered_map<std::string, Font*> m_fontMap;
    Font* m_currentFont = nullptr;

    friend class Renderer;
};

typedef std::map<GLchar, Character> FontFace;

class OpenGLFontSystem : public FontSystem
{
public:
    OpenGLFontSystem();

    //virtual int Initialize() override;
    virtual void RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color) override;
    virtual void LoadFont(Font& font) override;

private:
    std::map<GLchar, Character> Characters;

    std::unordered_map<std::string, FontFace> fontFaceMap;

    unsigned int VAO, VBO;
    Shader m_shader;
};