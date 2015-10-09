#include <vector>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <IL/il.h>
#include <glog/logging.h>
using std::vector;
using std::equal;
using std::copy;
using std::fill;
using std::abs;

template <class SignedIntType=int> SignedIntType ClampToUint8(SignedIntType x)
{
	const SignedIntType mask = 0xff;
	return (x&~mask)? ((~x)>>(sizeof(SignedIntType)*8-1) & mask): x;
}

inline int AddAndWrapIndex(int i, int added, int wrap_index)
{
	i += added;
	if (i >= wrap_index) {
		i -= wrap_index;
	}
	return i;
}

struct EdgeDetect3x3 {
	void FillBoundary(unsigned char* image_out, int w, int h, int bpp)
	{
		const int line_stride = bpp*w;
		fill(image_out, image_out+line_stride, 0);
		for (int y = 1; y < h-1; ++y) {
			fill(image_out, image_out+bpp, 0);
			fill(image_out+line_stride-bpp, image_out+line_stride, 0);
			image_out += line_stride;
		}
		fill(image_out, image_out+line_stride, 0);
	}

	void Run(unsigned char *image_in, unsigned char* image_out, int w, int h, int bpp)
	{
		CHECK_NE(w, 0) << "Width might not be 0";
		CHECK_NE(h, 0) << "Height might not be 0";
		FillBoundary(image_out, w, h, bpp);

		// The for loop is just a hint, you can decide
		// whether to use it
		const int line_stride = bpp*w;
	  int offset, current_response;
		for (int y = 1; y < h-1; ++y) {
			for (int x = bpp; x < line_stride-bpp; x+=bpp) {
			  offset = bpp*(w*y)+x;
      	for (int c = 0; c < bpp; c++) {
			  	current_response =
				    image_in[offset+c] * 4
					  -(image_in[offset+bpp+c]+image_in[offset-bpp+c]+image_in[offset-line_stride+c]+image_in[offset+line_stride+c]) * 2
					  +(image_in[offset+bpp+line_stride+c]+image_in[offset+bpp-line_stride+c]+image_in[offset-bpp+line_stride+c]+image_in[offset-bpp-line_stride+c]);
				  image_out[offset+c] = ClampToUint8(abs(current_response)*2);
        }
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
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	IL_CHECK_ERROR();

	// Read image
	ilLoadImage("lena_color.bmp");
	IL_CHECK_ERROR();
	auto bpp = ilGetInteger(IL_IMAGE_BPP); // bpp = byte per pixels
	auto w = ilGetInteger(IL_IMAGE_WIDTH);
	auto h = ilGetInteger(IL_IMAGE_HEIGHT);
	LOG(INFO) << "Load image width = " << w << ", height = " << h;
	ILubyte *color_img_ptr = ilGetData();
	vector<ILubyte> color_img_edge(w*h*bpp);
	IL_CHECK_ERROR();

	// Edge detection
	EdgeDetect3x3 ed;
	ed.Run(color_img_ptr, color_img_edge.data(), w, h, bpp);

	const int line_stride = w*bpp;
	for (int y = 1; y < h-1; ++y) {
		for (int x = 1; x < w-1; ++x) {
			int offset = bpp*(w*y+x);
			unsigned char *ptr_original = color_img_ptr + offset;
			unsigned char *ptr_yours = color_img_edge.data() + offset;
			for (int c = 0; c < bpp; c++) {
				int current_response =
					 ptr_original[0] * 4
					-(ptr_original[bpp]+ptr_original[-bpp]+ptr_original[-line_stride]+ptr_original[+line_stride]) * 2
					+(ptr_original[bpp+line_stride]+ptr_original[bpp-line_stride]+ptr_original[-bpp+line_stride]+ptr_original[-bpp-line_stride]);
				unsigned char answer = ClampToUint8(abs(current_response)*2);
				CHECK_EQ(answer, *ptr_yours) << "At (y,x,channel)" << y << ", " << x << ", " << c << ", ";

				++ptr_yours;
				++ptr_original;
			}
		}
	}
	copy(color_img_edge.begin(), color_img_edge.end(), color_img_ptr);

	// store image
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage("lena_r90.bmp");
	IL_CHECK_ERROR();

	ilDeleteImages(1, &image);
	return 0;
}
