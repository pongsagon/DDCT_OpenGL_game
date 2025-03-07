#include "FontSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Application.h"

OpenGLFontSystem::OpenGLFontSystem()
    : m_shader("Assets/Shaders/font.vert", "Assets/Shaders/font.frag")
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    s_defaultFont = std::make_unique<Font>("Assets/Fonts/RobotoMono-Regular.ttf");
    LoadFont(*s_defaultFont);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLFontSystem::LoadFont(Font& font)
{
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Could not init FreeType Library" << std::endl;
        return;
    }

    std::string fontName = font.GetPath();

    for (unsigned int size : Font::DEFAULT_FONT_SIZES)
    {
        GlyphMap glyphMap;

        FT_Face face;
        if (FT_New_Face(ft, fontName.c_str(), 0, &face)) {
            std::cout << "Failed to load font: " << fontName << std::endl;
            return;
        }
        else {
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, size);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                glyphMap.insert(std::pair<char, Character>(c, character));

            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        font.AddGlypMap(size, glyphMap);

        FT_Done_Face(face);
    }

    FT_Done_FreeType(ft);
}


void OpenGLFontSystem::RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color)
{
    unsigned int fontSize = static_cast<unsigned int>(scale);

    Font* usingFont = s_defaultFont.get();

    if (m_currentFont != nullptr)
        usingFont = m_currentFont;

    if (!usingFont->HasSize(fontSize))
        return;

    // activate corresponding render state	
    m_shader.use();
    m_shader.setVec4("textColor", color);

    glm::vec2 screenSize = Application::Get().GetWindowSize();

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenSize.x), 0.0f, static_cast<float>(screenSize.y));
    m_shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float x = position.x;
    float y = screenSize.y - position.y;

    float screenSizePercentage = 1.0f;
    float realScale = scale * screenSizePercentage / (float)fontSize;

    auto& Characters = usingFont->GetCharacters(fontSize);

    float xOffset = 0;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = std::floor(x + ch.Bearing.x * realScale - xOffset);
        float ypos = std::floor(y - (ch.Size.y - ch.Bearing.y) * realScale);

        float w = ch.Size.x * realScale;
        float h = ch.Size.y * realScale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * realScale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}