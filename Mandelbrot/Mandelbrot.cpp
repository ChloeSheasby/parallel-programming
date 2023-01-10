#include <iostream>
#include <fstream> /* for ofstream */
#include <complex> /* for fractal arithmetic */
#include <omp.h>
using namespace std;

/* The Mandelbrot set is defined by all numbers which do not diverge for fc(z) = z^2 + c,
 * where C is a complex number. Generally, we run the algorithm until we hit a cutoff number of iterations.
 * We can end the iterations early if we know that the sum of the complex coefficients is <= 4.
 * because if that happens we know it'll diverge.
 *
 * To draw the set, we map the real value to the x-axis, and the imaginary value to the y-axis.
 * We then use the number of iterations to escape to calculate the color of the pixel
 *
 * To convert the resulting PPM, you may use https://convertio.co/ppm-jpg/
 */

/**
 A linear function in 2 dimensions: returns a double as a function of (x,y).
*/
class linear2d_function {
public:
	double a, b, c;
	void set(double a_, double b_, double c_) { a = a_; b = b_; c = c_; }
	linear2d_function(double a_, double b_, double c_) { set(a_, b_, c_); }
	double evaluate(double x, double y) const { return x * a + y * b + c; }
};

void parallel();
void sequential();

int main()
{
	clock_t start, end;

	printf("Timing sequential...\n");
	start = clock();

	sequential();

	end = clock();
	printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

	printf("Timing parallel...\n");
	start = clock();

	parallel();

	end = clock();
	printf("Took %f seconds\n\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}

void sequential() {
	// Figure out how big an image we should render:
	int wid = 350, ht = 256;

	// Create a PPM output image file header:
	ofstream out("outSeq.ppm", ios_base::binary);
	out << "P6\n"
		<< wid << " " << ht << "\n"
		<< "255\n";

	// Set up coordinate system to render the Mandelbrot Set:
	double scale = 3.0 / wid;
	linear2d_function fx(scale, 0.0, -scale * wid / 2); // returns c given pixels 
	linear2d_function fy(0.0, scale, -1.0);

	for (int y = 0; y < ht; y++)
		for (int x = 0; x < wid; x++) {
			/* Walk this Mandelbrot Set pixel */
			typedef std::complex<double> COMPLEX;
			COMPLEX c(fx.evaluate(x, y), fy.evaluate(x, y));
			COMPLEX z(0.0);
			int count;
			enum { max_count = 100 };
			for (count = 0; count < max_count; count++) {
				z = z * z + c;
				if ((z.real() * z.real() + z.imag() * z.imag()) >= 4.0) break;
			}

			/* Figure out the output pixel color */
			unsigned char r, g, b;
			r = (unsigned char)(z.real() * (256 / 2.0));
			g = (unsigned char)(z.imag() * (256 / 2.0));
			b = (unsigned char)(((z.real() * z.real() + z.imag() * z.imag())) * 256);
			out << r << g << b;
		}

	out.close();
}

void parallel() {
	// Figure out how big an image we should render:
	int wid = 350, ht = 256;

	// Create a PPM output image file header:
	ofstream out("outPar.ppm", ios_base::binary);
	out << "P6\n"
		<< wid << " " << ht << "\n"
		<< "255\n";

	// Set up coordinate system to render the Mandelbrot Set:
	double scale = 3.0 / wid;
	linear2d_function fx(scale, 0.0, -scale * wid / 2); // returns c given pixels 
	linear2d_function fy(0.0, scale, -1.0);

	unsigned char r[350][256];
	unsigned char g[350][256];
	unsigned char b[350][256];

#pragma omp parallel for num_threads(omp_get_max_threads())
	for (int y = 0; y < ht; y++)
		for (int x = 0; x < wid; x++) {
			/* Walk this Mandelbrot Set pixel */
			typedef std::complex<double> COMPLEX;
			COMPLEX c(fx.evaluate(x, y), fy.evaluate(x, y));
			COMPLEX z(0.0);
			int count;
			enum { max_count = 100 };

			for (count = 0; count < max_count; count++) {
				z = z * z + c;
				if ((z.real() * z.real() + z.imag() * z.imag()) >= 4.0) break;
			}

			/* Figure out the output pixel color */
			r[x][y] = (unsigned char)(z.real() * (256 / 2.0));
			g[x][y] = (unsigned char)(z.imag() * (256 / 2.0));
			b[x][y] = (unsigned char)(((z.real() * z.real() + z.imag() * z.imag())) * 256);
		}

	for (int y = 0; y < ht; y++)
		for (int x = 0; x < wid; x++) {
			out << r[x][y] << g[x][y] << b[x][y];
		}

	out.close();
}