#include <vector>
#include <cstdio>
#include <IL/il.h>
#include <glog/logging.h>
using std::vector;

template <class SignedIntType=int> SignedIntType ClampToUint8(SignedIntType x)
{
	const SignedIntType mask = 0xff;
  return (x&~mask)? ((~x)>>(sizeof(SignedIntType)*8-1) & mask): x;
}

struct Convert {
	// color_img_ptr: pointer
	// gray_img_ptr: pointer
	// w: image width
	// h: image height
	void RGB2Gray(const unsigned char *color_img_ptr, unsigned char *gray_img_ptr, int w, int h)
	{
    int l;
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        l = 0.2126 * int(color_img_ptr[3*w*i+3*j]) + 
            0.7152 * int(color_img_ptr[3*w*i+3*j+1]) +
            0.0722 * int(color_img_ptr[3*w*i+3*j+2]);
        gray_img_ptr[w*i+j] = ClampToUint8(l);
      }
    }
	}
};

#define IL_CHECK_ERROR() {auto e = ilGetError();CHECK_EQ(e, IL_NO_ERROR)<<e;}
int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;
	ilInit();

	LOG(INFO) << "Using devil library version " << ilGetInteger(IL_VERSION_NUM);

	// Allocate images
	ILuint images[2]; // 0 for read, 1 for write
	ilGenImages(2, images);
	IL_CHECK_ERROR();

	// Read image
	ilBindImage(images[0]);
	ilLoadImage("lena_color.bmp");
	IL_CHECK_ERROR();
	auto bpp = ilGetInteger(IL_IMAGE_BPP); // bpp = byte per pixels
	CHECK_EQ(bpp, 3) << "BPP must be 3";
	auto w = ilGetInteger(IL_IMAGE_WIDTH);
	auto h = ilGetInteger(IL_IMAGE_HEIGHT);
	LOG(INFO) << "Load image width = " << w << ", height = " << h;
	ILubyte *color_img_ptr = ilGetData();
	IL_CHECK_ERROR();

	// Allocate image for store
	ilBindImage(images[1]);
	IL_CHECK_ERROR();
	ilTexImage(w, h, 1, bpp, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);
	ILubyte *gray_img_ptr = ilGetData();
	IL_CHECK_ERROR();

	// Convert!
	Convert c;
	c.RGB2Gray(color_img_ptr, gray_img_ptr, w, h);

	// store image
	ilBindImage(images[1]);
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage("lena_gray.bmp");
	IL_CHECK_ERROR();

	ilDeleteImages(2, images);
	return 0;
}
