// --------------------------------------------------------------
// SL 2018-01-02
// A simple, easily hackable CPU surface voxelizer
// MIT-license
// (c) Sylvain Lefebvre, https://github.com/sylefeb
// --------------------------------------------------------------

/*

Takes as input a file 'model.stl' from the source directory.
Outputs a voxel file named 'out.slab.vox' that can be imported
by 'MagicaVoxel' https://ephtracy.github.io/

Change VOXEL_RESOLUTION  to fit your needs.
Set    VOXEL_FILL_INSIDE to 1 to fill in the interior
Set    VOXEL_ROBUST_FILL to 1 to fill in the interior using
	   a voting scheme (more robust, slower)

The basic principle is to rasterize triangles using three 2D axis
aligned grids, using integer arithmetic (fixed floating point)
for robust triangle interior checks.

Very simple and quite efficient despite a straightforward implementation.
Higher resolutions could easily be reached by not storing the
voxels as a 3D array of booleans (e.g. use blocking or an octree).

For the inside fill to work properly, the mesh has to be perfectly
watertight, with exactly matching vertices between neighboring
verticies.

*/

#include <LibSL/LibSL.h>

#include <iostream>
#include <algorithm>
#include <fstream>
#include <queue>
#include <Windows.h>
using namespace std;

// --------------------------------------------------------------

#define VOXEL_RESOLUTION  128
#define VOXEL_FILL_INSIDE 1
#define VOXEL_ROBUST_FILL 1

// --------------------------------------------------------------

#define FP_POW    16 //16
#define FP_SCALE  (1<<FP_POW)
#define BOX_SCALE v3f(VOXEL_RESOLUTION*FP_SCALE)

#define ALONG_X  1  //1
#define ALONG_Y  2  //2
#define ALONG_Z  4  //4
#define INSIDE   8  //8
#define INSIDE_X 16 //16
#define INSIDE_Y 32 //32
#define INSIDE_Z 64 //64

// --------------------------------------------------------------

// saves a voxel file (.slab.vox format, can be imported by MagicaVoxel)
void saveAsVox(const char* fname, const Array3D<uchar>& voxs)
{
	Array<v3b> palette(256); // RGB palette
	palette.fill(0);
	palette[123] = v3b(127, 0, 127);
	palette[124] = v3b(255, 0, 0);
	palette[125] = v3b(0, 255, 0);
	palette[126] = v3b(0, 0, 255);
	palette[127] = v3b(255, 255, 255);
	FILE* f;
	f = fopen(fname, "wb");
	sl_assert(f != NULL);
	long sx = voxs.xsize(), sy = voxs.ysize(), sz = voxs.zsize();
	fwrite(&sx, 4, 1, f);
	fwrite(&sy, 4, 1, f);
	fwrite(&sz, 4, 1, f);
	ForRangeReverse(i, sx - 1, 0)
	{
		ForIndex(j, sy)
		{
			ForRangeReverse(k, sz - 1, 0)
			{
				uchar v = voxs.at(i, j, k);
				uchar pal = v != 0 ? 127 : 255;
				if (v == INSIDE)
				{
					pal = 123;
				}
				fwrite(&pal, sizeof(uchar), 1, f);
			}
		}
	}
	fwrite(palette.raw(), sizeof(v3b), 256, f);
	fclose(f);
}

// --------------------------------------------------------------

inline bool isInTriangle(int i, int j, const v3i& p0, const v3i& p1, const v3i& p2, int& _depth)
{
	v2i delta_p0 = v2i(i, j) - v2i(p0);
	v2i delta_p1 = v2i(i, j) - v2i(p1);
	v2i delta_p2 = v2i(i, j) - v2i(p2);
	v2i delta10 = v2i(p1) - v2i(p0);
	v2i delta21 = v2i(p2) - v2i(p1);
	v2i delta02 = v2i(p0) - v2i(p2);

	int64_t c0 = static_cast<int64_t>(delta_p0[0]) * static_cast<int64_t>(delta10[1]) - static_cast<int64_t>(delta_p0[1]
	) * static_cast<int64_t>(delta10[0]);
	int64_t c1 = static_cast<int64_t>(delta_p1[0]) * static_cast<int64_t>(delta21[1]) - static_cast<int64_t>(delta_p1[1]
	) * static_cast<int64_t>(delta21[0]);
	int64_t c2 = static_cast<int64_t>(delta_p2[0]) * static_cast<int64_t>(delta02[1]) - static_cast<int64_t>(delta_p2[1]
	) * static_cast<int64_t>(delta02[0]);
	bool inside = (c0 <= 0 && c1 <= 0 && c2 <= 0) || (c0 >= 0 && c1 >= 0 && c2 >= 0);

	if (inside)
	{
		int64_t area = c0 + c1 + c2;
		int64_t b0 = (c1 << 10) / area;
		int64_t b1 = (c2 << 10) / area;
		int64_t b2 = (1 << 10) - b0 - b1;
		_depth = static_cast<int>((b0 * p0[2] + b1 * p1[2] + b2 * p2[2]) >> 10);
	}
	return inside;
}

// --------------------------------------------------------------

class swizzle_xyz
{
public:
	v3i forward(const v3i& v) const { return v; }
	v3i backward(const v3i& v) const { return v; }
	int along() const { return ALONG_Z; }
};

class swizzle_zxy
{
public:
	v3i forward(const v3i& v) const { return v3i(v[2], v[0], v[1]); }
	v3i backward(const v3i& v) const { return v3i(v[1], v[2], v[0]); }
	uchar along() const { return ALONG_Y; }
};

class swizzle_yzx
{
public:
	v3i forward(const v3i& v) const { return v3i(v[1], v[2], v[0]); }
	v3i backward(const v3i& v) const { return v3i(v[2], v[0], v[1]); }
	uchar along() const { return ALONG_X; }
};

// --------------------------------------------------------------

template <class S>
void rasterize(
	const v3u& tri,
	const std::vector<v3i>& pts,
	Array3D<uchar>& _voxs)
{
	const S swizzler;
	v3i tripts[3] = {
		swizzler.forward(pts[tri[0]]),
		swizzler.forward(pts[tri[1]]),
		swizzler.forward(pts[tri[2]])
	};
	// check if triangle is valid
	v2i delta10 = v2i(tripts[1]) - v2i(tripts[0]);
	v2i delta21 = v2i(tripts[2]) - v2i(tripts[1]);
	v2i delta02 = v2i(tripts[0]) - v2i(tripts[2]);
	if (delta10 == v2i(0)) return;
	if (delta21 == v2i(0)) return;
	if (delta02 == v2i(0)) return;
	if (delta02[0] * delta10[1] - delta02[1] * delta10[0] == 0) return;
	// proceed
	AAB<2, int> pixbx;
	pixbx.addPoint(v2i(tripts[0]) / FP_SCALE);
	pixbx.addPoint(v2i(tripts[1]) / FP_SCALE);
	pixbx.addPoint(v2i(tripts[2]) / FP_SCALE);
	for (int j = pixbx.minCorner()[1]; j <= pixbx.maxCorner()[1]; j++)
	{
		for (int i = pixbx.minCorner()[0]; i <= pixbx.maxCorner()[0]; i++)
		{
			int depth;
			if (isInTriangle(
				(i << FP_POW) + (1 << (FP_POW - 1)), // centered
				(j << FP_POW) + (1 << (FP_POW - 1)), // centered
				tripts[0], tripts[1], tripts[2], depth))
			{
				v3i vx = swizzler.backward(v3i(i, j, depth >> FP_POW));
				// tag the voxel as occupied
				// NOTE: voxels are likely to be hit multiple times (e.g. thin features)
				//       we flip the bit every time a hit occurs in a voxel
				_voxs.at(vx[0], vx[1], vx[2]) = (_voxs.at(vx[0], vx[1], vx[2]) ^ swizzler.along());
			}
		}
	}
}

// --------------------------------------------------------------

// This version is more robust by using all three direction
// and voting among them to decide what is filled or not
void fillInsideVoting(Array3D<uchar>& _voxs)
{
	// along x
	ForIndex(k, _voxs.zsize())
	{
		ForIndex(j, _voxs.ysize())
		{
			bool inside = false;
			ForIndex(i, _voxs.xsize())
			{
				if (_voxs.at(i, j, k) & ALONG_X)
				{
					inside = !inside;
				}
				if (inside)
				{
					_voxs.at(i, j, k) |= INSIDE_X;
				}
			}
		}
	}
	// along y
	ForIndex(k, _voxs.zsize())
	{
		ForIndex(j, _voxs.xsize())
		{
			bool inside = false;
			ForIndex(i, _voxs.ysize())
			{
				if (_voxs.at(j, i, k) & ALONG_Y)
				{
					inside = !inside;
				}
				if (inside)
				{
					_voxs.at(j, i, k) |= INSIDE_Y;
				}
			}
		}
	}
	// along z
	ForIndex(k, _voxs.ysize())
	{
		ForIndex(j, _voxs.xsize())
		{
			bool inside = false;
			ForIndex(i, _voxs.zsize())
			{
				if (_voxs.at(j, k, i) & ALONG_Z)
				{
					inside = !inside;
				}
				if (inside)
				{
					_voxs.at(j, k, i) |= INSIDE_Z;
				}
			}
		}
	}
	// voting
	ForArray3D(_voxs, i, j, k)
	{
		uchar v = _voxs.at(i, j, k);
		int votes =
			((v & INSIDE_X) ? 1 : 0)
			+ ((v & INSIDE_Y) ? 1 : 0)
			+ ((v & INSIDE_Z) ? 1 : 0);
		// clean
		_voxs.at(i, j, k) &= ~(INSIDE_X | INSIDE_Y | INSIDE_Z);
		if (votes > 1)
		{
			// tag as inside
			_voxs.at(i, j, k) |= INSIDE;
		}
	}
}

// --------------------------------------------------------------

void fillInside(Array3D<uchar>& _voxs)
{
	ForIndex(k, _voxs.zsize())
	{
		ForIndex(j, _voxs.ysize())
		{
			bool inside = false;
			ForIndex(i, _voxs.xsize())
			{
				if (_voxs.at(i, j, k) & ALONG_X)
				{
					inside = !inside;
				}
				if (inside)
				{
					_voxs.at(i, j, k) |= INSIDE;
				}
			}
		}
	}
}

vector<string> get_all_files_names_within_folder(string folder)
{
    vector<string> names;
    string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

// --------------------------------------------------------------

void main()
{
	cout << "Input path to directory: ";
	string inputPath;
	cin >> inputPath;

	cout << "Input path to output directory: ";
	string outputPath;
	cin >> outputPath;

	cout << "What'a hell is that: ";
	string type;
	cin >> type;

	int typeCount = 0;
	
	vector<string> allFiles = get_all_files_names_within_folder(inputPath);
	for (string file : allFiles)
	{
		//std::cout << file << std::endl;
		try
		{
			// load triangle mesh

			//single file
			/*cout << "Input path (max 100 symbols): ";
			char* inputPath = new char[100];
			cin >> inputPath;
			TriangleMesh* mesh(loadTriangleMesh(inputPath));*/

			//foreach
			TriangleMesh* mesh(loadTriangleMesh((inputPath + "/" + file).c_str()));

			//default
			//TriangleMesh* mesh(loadTriangleMesh("./2.stl"));
			// produce (fixed fp) integer vertices and triangles
			std::vector<v3i> pts;
			std::vector<v3u> tris;
			{
				float factor = 0.95f;
				m4x4f boxtrsf = scaleMatrix(BOX_SCALE)
					* scaleMatrix(v3f(1.f) / tupleMax(mesh->bbox().extent()))
					* translationMatrix((1 - factor) * 0.5f * mesh->bbox().extent())
					* scaleMatrix(v3f(factor))
					* translationMatrix(-mesh->bbox().minCorner());

				// transform vertices
				pts.resize(mesh->numVertices());
				ForIndex(p, mesh->numVertices())
				{
					v3f pt = mesh->posAt(p);
					v3f bxpt = boxtrsf.mulPoint(pt);
					v3i ipt = v3i(clamp(round(bxpt), v3f(0.0f), BOX_SCALE - v3f(1.0f)));
					pts[p] = ipt;
				}
				// prepare triangles
				tris.reserve(mesh->numTriangles());
				ForIndex(t, mesh->numTriangles())
				{
					v3u tri = mesh->triangleAt(t);
					tris.push_back(tri);
				}
			}

			// rasterize into voxels
			v3u resolution(
				mesh->bbox().extent() / tupleMax(mesh->bbox().extent()) * static_cast<float>(VOXEL_RESOLUTION));
			Array3D<uchar> voxs(resolution);
			voxs.fill(0);
			{
				Timer tm("rasterization");
				Console::progressTextInit(static_cast<int>(tris.size()));
				ForIndex(t, tris.size())
				{
					Console::progressTextUpdate(t);
					rasterize<swizzle_xyz>(tris[t], pts, voxs); // xy view
					rasterize<swizzle_yzx>(tris[t], pts, voxs); // yz view
					rasterize<swizzle_zxy>(tris[t], pts, voxs); // zx view
				}
				Console::progressTextEnd();
				cerr << endl;
			}

			// add inner voxels
#if VOXEL_FILL_INSIDE
			{
				Timer tm("fill");
				cerr << "filling in/out ... ";
#if VOXEL_ROBUST_FILL
				fillInsideVoting(voxs);
#else
				fillInside(voxs);
#endif
				cerr << " done." << endl;
			}
#endif

			// save the result
			//li ��� ��� ����
			/*cout << "Output path (max 100 symbols): ";
			char* outputPath = new char[100];
			cin >> outputPath;
			saveAsVox(outputPath, voxs);*/
			
			string fileName = outputPath + "/" + type + "_" + to_string(typeCount) + ".vox";
			cout << "Saving " + fileName + " : ";
			saveAsVox(const_cast<char*>(fileName.c_str()), voxs);
			typeCount++;

			// report some stats
			/*int num_in_vox = 0;
			ForArray3D(voxs, i, j, k)
			{
				if (voxs.at(i, j, k) > 0)
				{
					num_in_vox++;
				}
			}
			cerr << "number of set voxels: " << num_in_vox << endl;	*/		
		}
		catch (Fatal& e)
		{
			cout << "[ERROR] " << e.message() << endl;
			cerr << "[ERROR] " << e.message() << endl;
		}
	}
}

/* -------------------------------------------------------- */
