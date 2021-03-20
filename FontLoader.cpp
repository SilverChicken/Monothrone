#include "FontLoader.h"

FontLoader::FontLoader()
{
	CurX = CurY = 0;
	Rd = Gr = Bl = 1.0f;
	InvertYAxis = false;
}

FontLoader::~FontLoader()
{

}

bool FontLoader::Load(const char * fname)
{
	char *dat, *img;
	std::fstream in;
	unsigned long fileSize;
	char bpp;
	int ImgX, ImgY;

	in.open(fname, std::ios_base::binary | std::ios_base::in);

	if (in.fail())
		return false;

	// Get Filesize
	in.seekg(0, std::ios_base::end);
	fileSize = in.tellg();
	in.seekg(0, std::ios_base::beg);

	// allocate space for file data
	dat = new char[fileSize];

	// Read filedata
	if (!dat)
		return false;

	in.read(dat, fileSize);

	if (in.fail())
	{
		delete[] dat;
		in.close();
		return false;
	}

	in.close();

	// Check ID is 'BFF2'
	if ((unsigned char)dat[0] != 0xBF || (unsigned char)dat[1] != 0xF2)
	{
		delete[] dat;
		return false;
	}

	// Grab the rest of the header
	memcpy(&ImgX, &dat[2], sizeof(int));
	memcpy(&ImgY, &dat[6], sizeof(int));
	memcpy(&CellX, &dat[10], sizeof(int));
	memcpy(&CellY, &dat[14], sizeof(int));
	bpp = dat[18];
	Base = dat[19];

	// Check filesize
	if (fileSize != ((MAP_DATA_OFFSET)+((ImgX*ImgY)*(bpp / 8))))
		return false;

	// Calculate font params
	RowPitch = ImgX / CellX;
	ColFactor = (float)CellX / (float)ImgX;
	RowFactor = (float)CellY / (float)ImgY;
	YOffset = CellY;

	// Determine blending options based on BPP
	switch (bpp)
	{
	case 8: // Greyscale
		RenderStyle = BFG_RS_ALPHA;
		break;

	case 24: // RGB
		RenderStyle = BFG_RS_RGB;
		break;

	case 32: // RGBA
		RenderStyle = BFG_RS_RGBA;
		break;

	default: // Unsupported BPP
		delete[] dat;
		return false;
		break;
	}

	// Allocate space for image
	img = new char[(ImgX*ImgY)*(bpp / 8)];

	if (!img)
	{
		delete[] dat;
		return false;
	}

	// Grab char widths
	memcpy(Width, &dat[WIDTH_DATA_OFFSET], 256);

	// Grab image data
	memcpy(img, &dat[MAP_DATA_OFFSET], (ImgX*ImgY)*(bpp / 8));

	// Create Texture
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	// Fonts should be rendered at native resolution so no need for texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Stop chararcters from bleeding over edges
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Tex creation params are dependent on BPP
	switch (RenderStyle)
	{
	case BFG_RS_ALPHA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, ImgX, ImgY, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, img);
		break;

	case BFG_RS_RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImgX, ImgY, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		break;

	case BFG_RS_RGBA:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImgX, ImgY, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		break;
	}

	// Clean up
	delete[] img;
	delete[] dat;

	return true;
}

void FontLoader::SetScreen(int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (InvertYAxis)
		glOrtho(0, x, y, 0, -1, 1);
	else
		glOrtho(0, x, 0, y, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FontLoader::SetCursor(int x, int y)
{
	CurX = x;
	CurY = y;
}

void FontLoader::SetColor(float Red, float Green, float Blue)
{
	Rd = Red;
	Gr = Green;
	Bl = Blue;
}

void FontLoader::ReverseYAxis(bool State)
{
	if (State)
		YOffset = -CellY;
	else
		YOffset = CellY;

	InvertYAxis = State;
}

void FontLoader::Select()
{
	glEnable(GL_TEXTURE_2D);
	Bind();
	//SetBlend();
}

void FontLoader::Bind()
{
	glBindTexture(GL_TEXTURE_2D, TexID);
}

void FontLoader::SetBlend()
{
	glColor3f(Rd, Gr, Bl);

	switch (RenderStyle)
	{
	case BFG_RS_ALPHA: // 8Bit
		glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
		glEnable(GL_BLEND);
		break;

	case BFG_RS_RGB:   // 24Bit
		glDisable(GL_BLEND);
		break;

	case BFG_RS_RGBA:  // 32Bit
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		break;
	}
}

void FontLoader::Print(char * Text)
{
	int sLen, Loop;
	int Row, Col;
	float U, V, U1, V1;

	sLen = (int)strnlen(Text, BFG_MAXSTRING);

	glBegin(GL_QUADS);

	for (Loop = 0; Loop != sLen; ++Loop)
	{
		Row = (Text[Loop] - Base) / RowPitch;
		Col = (Text[Loop] - Base) - Row * RowPitch;

		U = Col * ColFactor;
		V = Row * RowFactor;
		U1 = U + ColFactor;
		V1 = V + RowFactor;

		glTexCoord2f(U, V1);  glVertex2i(CurX, CurY);
		glTexCoord2f(U1, V1);  glVertex2i(CurX + CellX, CurY);
		glTexCoord2f(U1, V); glVertex2i(CurX + CellX, CurY + CellY);
		glTexCoord2f(U, V); glVertex2i(CurX, CurY + CellY);

		CurX += Width[Text[Loop]];
	}
	glEnd();
}

void FontLoader::Print(char * Text, int x, int y)
{
	CurX = x;
	CurY = y;

	int sLen, Loop;
	int Row, Col;
	float U, V, U1, V1;

	sLen = (int)strnlen(Text, BFG_MAXSTRING);

	glBegin(GL_QUADS);

	for (Loop = 0; Loop != sLen; ++Loop)
	{
		Row = (Text[Loop] - Base) / RowPitch;
		Col = (Text[Loop] - Base) - Row * RowPitch;

		U = Col * ColFactor;
		V = Row * RowFactor;
		U1 = U + ColFactor;
		V1 = V + RowFactor;

		glTexCoord2f(U, V1);  glVertex2i(CurX, CurY);
		glTexCoord2f(U1, V1);  glVertex2i(CurX + CellX, CurY);
		glTexCoord2f(U1, V); glVertex2i(CurX + CellX, CurY + CellY);
		glTexCoord2f(U, V); glVertex2i(CurX, CurY + CellY);

		CurX += Width[Text[Loop]];
	}
	glEnd();
}

int FontLoader::GetWidth(char * Text)
{
	int Loop, sLen, Size;

	// How many chars in string?
	sLen = (int)strnlen(Text, BFG_MAXSTRING);

	// Add up all width values
	for (Loop = 0, Size = 0; Loop != sLen; ++Loop)
	{
		Size += Width[Text[Loop]];
	}

	return Size;
}


