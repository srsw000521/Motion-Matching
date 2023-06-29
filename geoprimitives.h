#if !defined(_GEOPRIVITIVES_INCLUDED_)
#define _GEOPRIVITIVES_INCLUDED_

#pragma warning(disable : 4996)


#include "MathPrim.h"

class Edge : public CObject
{
public:

	DECLARE_SERIAL( Edge )

	Edge(){
	};
	Edge(const Edge & a)
	{
		verticesIndex[0] = a.verticesIndex[0];
		verticesIndex[1] = a.verticesIndex[1];
		triangleIndex[0] = a.triangleIndex[0];
		triangleIndex[1] = a.triangleIndex[1];
	};
	Edge & operator = (const Edge & a)
	{
		verticesIndex[0] = a.verticesIndex[0];
		verticesIndex[1] = a.verticesIndex[1];
		triangleIndex[0] = a.triangleIndex[0];
		triangleIndex[1] = a.triangleIndex[1];
		return (*this);
	};
	
	bool IsSame(const Edge & a)
	{
		if(verticesIndex[0] == a.verticesIndex[0] && verticesIndex[1] == a.verticesIndex[1]) return true;
		if(verticesIndex[1] == a.verticesIndex[0] && verticesIndex[0] == a.verticesIndex[1]) return true;

		return false;
	};
	void PrintEdge(FILE * file)
	{
		if(verticesIndex[0]<0) verticesIndex[0] = -1;
		if(verticesIndex[1]<0) verticesIndex[1] = -1;
		if(triangleIndex[0]<0) triangleIndex[0] = -1;
		if(triangleIndex[1]<0) triangleIndex[1] = -1;

		
		
		fprintf(file, "%d %d ", verticesIndex[0], verticesIndex[1]);
		fprintf(file, "%d %d ", triangleIndex[0], triangleIndex[1]);
	};
	void LoadEdge(FILE * file)
	{
		fscanf(file, "%d %d", &verticesIndex[0], &verticesIndex[1]);
		fscanf(file, "%d %d", &triangleIndex[0], &triangleIndex[1]);
	};

	int verticesIndex[2];
	int triangleIndex[2];

	void Serialize(CArchive & ar)
	{
		CObject::Serialize(ar);
		if(ar.IsStoring())
		{
			ar<<verticesIndex[0]<<verticesIndex[1];
			ar<<triangleIndex[0]<<triangleIndex[1];
		}
		else
		{
			ar>>verticesIndex[0]>>verticesIndex[1];
			ar>>triangleIndex[0]>>triangleIndex[1];
		}
	}
};




class Triangle : public CObject
{
public:
	DECLARE_SERIAL( Triangle )

	Triangle(){	
		verticesIndex[0] = -1;
		verticesIndex[1] = -1;
		verticesIndex[2] = -1;
		edgesIndex[0] = -1;
		edgesIndex[1] = -1;
		edgesIndex[2] = -1;
		normalsIndex[0] = -1;
		normalsIndex[1] = -1;
		normalsIndex[2] = -1;
		color = Vector3f(0.5,0.5f,0.5f);
		neighborTriIndex[0] = -1;
		neighborTriIndex[1] = -1;
		neighborTriIndex[2] = -1;
	
		textureIndex[0] = -1;
		textureIndex[1] = -1;
		textureIndex[2] = -1;


	};

	Triangle(const Triangle & a)
	{
		int j;
		for(j=0; j<3; j++)
		{
			verticesIndex[j] = a.verticesIndex[j];
			edgesIndex[j] = a.edgesIndex[j];
			normalsIndex[j] = a.normalsIndex[j];
			vertices[j] = a.vertices[j];
			normals[j] = a.normals[j];
			edges[j] = a.edges[j];
			textureIndex[j] = a.textureIndex[j];
			neighborTriIndex[j] = a.neighborTriIndex[j];
		}
		normal = a.normal;
		color = a.color;
		area = a.area;
		vaild = a.vaild;
		

	};
	Triangle& operator = (const Triangle & a )
	{
		int j;
		for(j=0; j<3; j++)
		{
			verticesIndex[j] = a.verticesIndex[j];
			edgesIndex[j] = a.edgesIndex[j];
			normalsIndex[j] = a.normalsIndex[j];
			vertices[j] = a.vertices[j];
			normals[j] = a.normals[j];
			edges[j] = a.edges[j];
			textureIndex[j] = a.textureIndex[j];
			neighborTriIndex[j] = a.neighborTriIndex[j];
		}
		normal = a.normal;
		color = a.color;
		area = a.area;
		vaild = a.vaild;
		return (*this);
	};

	Quat GetRotation()
	{
		Quat q;
		Vector3f a = vertices[0] - vertices[1];
		Vector3f b = vertices[2] - vertices[1];

		Vector3f z = b*a;		//			normal direction
		Vector3f x = (a+b)/2.0f;
		Vector3f y = z*x;

		x = x/x.length();
		y = y/y.length();
		z = z/z.length();

		MyMatrix mat;
		mat.Set(x, y, z);
		q.MatrixToQuat(mat);

//		matrix mat;
//		mat = matrix(x,y,z);
//		q = Matrix2Quat(mat);

		return q;
	};
	Vector3f GetCenter()
	{
		return (vertices[0]+vertices[1]+vertices[2])/3.0f;
	};

	bool IsMemberVertex(int verInd)
	{
		return (verticesIndex[0] == verInd || verticesIndex[1] == verInd || verticesIndex[2] == verInd);
	};
	
	bool IsNeighbor(const Triangle & a)
	{
		int i,j;
		for(i=0; i<3; i++)
			for(j=0; j<3; j++)
				if(edges[i].IsSame(a.edges[j])) return true;
//				if(edgesIndex[i]==a.edgesIndex[j]) return true;

		return false;
	};

	int GetClosestVertexIndexFrom(Vector3f point)
	{
		int out_index = -1;
		
		float dist = FLT_MAX;
		Vector3f pt;
		for(int k = 0; k<3; k++)
		{
			pt = vertices[k];
			pt -= point;
			float leng = pt.Length();
			if( leng < dist)
			{
				dist = leng;
				out_index= verticesIndex[k];
			}
		}
		return out_index;

	}


	Vector3f GetClosestFrom(Vector3f point)
	{
		Vector3f P = point;
		Vector3f B = vertices[1];
		Vector3f E0 = vertices[0]-B;
		Vector3f E1 = vertices[2]-B;
		Vector3f D = B-P;
		float a = E0 % E0;
		float b = E0 % E1;
		float c = E1 % E1;
		float d = E0 % D;
		float e = E1 % D;
		float f = D % D;

		float det = a*c-b*b;
		float s = b*e-c*d;
		float t = b*d-a*e;
		if( s+t<=det )
		{
			if(s<0.0f)
			{
				if(t<0.0f)
				{
					//region 4
					s = 0.0f;
					t = 0.0f;
				}
				else
				{
					//region 3
					s=0.0f;
					t = (e>= 0.0f ? 0.0f: (-e>=c ? 1.0f :-e/c));
				}
			}
			else if(t<0.0f)
			{
				//region 5
					t=0.0f;
					s = (d>= 0.0f ? 0.0f: (-d>=a ? 1.0f :-d/a));
			}
			else
			{
				//region 0
				float invDet = 1.0f/det;
				s *= invDet;
				t *= invDet;
			}
		}
		else
		{
			if (s<0.0f)
			{
				//region 2
				s = 0.0f;
				t = 1.0f;
			}
			else if (t<0.0f)
			{
				//region 6
				s = 1.0f;
				t = 0.0f;
			}
			else
			{
				//region 1
				float numer = c+e-b-d;
				if(numer <=0.0f)
					s=0.0f;
				else
				{
					float denom = a-2.0f*b+c;
					s = (numer >= denom ? 1.0f: numer/denom );
				}
				t = 1.0f-s;
			}
		}
		
		Vector3f Q = B + s*E0 + t*E1;
//		Vector3f PQ = Q-P;
//		dist = PQ%PQ;
		//return dist;
		return Q;
	};

	Vector3f GetClosestPointWeight(Vector3f point)
	{
		Vector3f P = point;
		Vector3f B = vertices[1];
		Vector3f E0 = vertices[0]-B;
		Vector3f E1 = vertices[2]-B;
		Vector3f D = B-P;
		float a = E0 % E0;
		float b = E0 % E1;
		float c = E1 % E1;
		float d = E0 % D;
		float e = E1 % D;
		float f = D % D;

		float det = a*c-b*b;
		float s = b*e-c*d;
		float t = b*d-a*e;
		if( s+t<=det )
		{
			if(s<0.0f)
			{
				if(t<0.0f)
				{
					//region 4
					s = 0.0f;
					t = 0.0f;
				}
				else
				{
					//region 3
					s=0.0f;
					t = (e>= 0.0f ? 0.0f: (-e>=c ? 1.0f :-e/c));
				}
			}
			else if(t<0.0f)
			{
				//region 5
					t=0.0f;
					s = (d>= 0.0f ? 0.0f: (-d>=a ? 1.0f :-d/a));
			}
			else
			{
				//region 0
				float invDet = 1.0f/det;
				s *= invDet;
				t *= invDet;
			}
		}
		else
		{
			if (s<0.0f)
			{
				//region 2
				s = 0.0f;
				t = 1.0f;
			}
			else if (t<0.0f)
			{
				//region 6
				s = 1.0f;
				t = 0.0f;
			}
			else
			{
				//region 1
				float numer = c+e-b-d;
				if(numer <=0.0f)
					s=0.0f;
				else
				{
					float denom = a-2.0f*b+c;
					s = (numer >= denom ? 1.0f: numer/denom );
				}
				t = 1.0f-s;
			}
		}
		
//		Vector3f B = vertices[1];
//		Vector3f E0 = vertices[0]-B;
//		Vector3f E1 = vertices[2]-B;
//		Vector3f Q = B + s*E0 + t*E1;

		Vector3f Q = Vector3f(s, 1.0f-s-t, t);
		return Q;
	};



// intersect_RayTriangle(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
	int intersect_RayTriangle( Vector3f P0, Vector3f P1,  Vector3f * I )
	{
		Vector3f    u, v, n;             // triangle vectors
		Vector3f    dir, w0, w;          // ray vectors
		float     r, a, b;             // params to calc ray-plane intersect

		Vector3f V0 = vertices[0];
		Vector3f V1 = vertices[1];
		Vector3f V2 = vertices[2];
				

		// get triangle edge vectors and plane normal
		u = V1 - V0;
		v = V2 - V0;
		n.Cross(u,v);	               // cross product
		if (n.Length() == 0.0f )            // triangle is degenerate
			return -1;                 // do not deal with this case

		dir = P1 - P0;             // ray direction vector

		w0 = P0 - V0;
		a = - (n % w0);
		b = n%dir;
		if (fabs(b) < 0.00001f) {     // ray is parallel to triangle plane
			if (a == 0)                // ray lies in triangle plane
				return 2;
			else return 0;             // ray disjoint from plane
		}

		// get intersect point of ray with triangle plane
		r = a / b;
		if (r < 0.0)                   // ray goes away from triangle
			return 0;                  // => no intersect
		// for a segment, also test if (r > 1.0) => no intersect

		* I = P0 + r * dir;           // intersect point of ray and plane

		// is I inside T?
		float    uu, uv, vv, wu, wv, D;
		uu = (u%u);
		uv = (u%v);
		vv = (v%v);
		w = *I - V0;
		wu = (w%u);
		wv = (w%v);
		D = uv * uv - uu * vv;

		// get and test parametric coords
		float s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)        // I is outside T
			return 0;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return 0;

		return 1;                      // I is in T
	}

	void FlipFace()
	{
		std::swap(verticesIndex[0], verticesIndex[1]);
	}



	Vector3f GetBarycentricCoord(Vector3f in)
	{
		Vector3f a,b;
		float area, s[3];
		Vector3f coord;
		a = vertices[1]-in;
		b = vertices[2]-in;
		s[0] = Vector3f(a*b).length();
		a = vertices[2]-in;
		b = vertices[0]-in;
		s[1] = Vector3f(a*b).length();
		a = vertices[0]-in;
		b = vertices[1]-in;
		s[2] = Vector3f(a*b).length();
		area = s[0]+s[1]+s[2];
		return Vector3f(s[0], s[1], s[2])/area;
	};

	int FindNextVertex(int vert_ind)
	{
		int i;
		int ind = -2;
		for(i=0; i<3; i++)
			if(vert_ind == verticesIndex[i])
			{
				ind = i;
				break;
			}
		ind ++;
		if(ind<0) return ind;
		if(ind>2) ind = 0;
		return verticesIndex[ind];
	};
	void PrintFace(FILE * file)
	{
		int j;
		for(j=0; j<3; j++)
		{
			fprintf(file, "%d %d %d %d ", verticesIndex[j], edgesIndex[j], normalsIndex[j], textureIndex[j]);
			edges[j].PrintEdge(file);
		}
		fprintf(file, "%f %f %f ", color[0], color[1], color[2]);
		if(area<0.0f) area = 0.0f;
		fprintf(file, "%f ", area);
		fprintf(file, "\n");
			
	};
	void LoadFace(FILE * file)
	{
		int j;
		for(j=0; j<3; j++)
		{
			fscanf(file, "%d %d %d %d ", &verticesIndex[j], &edgesIndex[j], &normalsIndex[j], &textureIndex[j]);
			edges[j].LoadEdge(file);
		}
		fscanf(file, "%f %f %f", &color[0], &color[1], &color[2]);
		fscanf(file, "%f", &area);
		
	};

	float GetCenterDistaceFromTriangle(Triangle & in)
	{
		Vector3f c1 = GetCenter();
		Vector3f c2 = in.GetCenter();
		return (c1-c2).Length();
	}

	void Serialize(CArchive & ar)
	{
		CObject::Serialize(ar);
		if(ar.IsStoring())
		{
			ar<<verticesIndex[0]<<verticesIndex[1]<<verticesIndex[2];
			ar<<edgesIndex[0]<<edgesIndex[1]<<edgesIndex[2];
			ar<<normalsIndex[0]<<normalsIndex[1]<<normalsIndex[2];
			for(int i=0; i<3; i++)
				ar<<vertices[i].x<<vertices[i].y<<vertices[i].z;
			for(int i=0; i<3; i++)
				ar<<normals[i].x<<normals[i].y<<normals[i].z;
			ar<<normal.x<<normal.y<<normal.z;
			ar<<color.x<<color.y<<color.z;
			ar<<textureIndex[0]<<textureIndex[1]<<textureIndex[2];

			for(int i=0; i<3; i++)
				edges[i].Serialize(ar);

			ar<<neighborTriIndex[0]<<neighborTriIndex[1]<<neighborTriIndex[2];
			ar<<area;
			ar<<vaild;
		}
		else
		{
			ar>>verticesIndex[0]>>verticesIndex[1]>>verticesIndex[2];
			ar>>edgesIndex[0]>>edgesIndex[1]>>edgesIndex[2];
			ar>>normalsIndex[0]>>normalsIndex[1]>>normalsIndex[2];
			for(int i=0; i<3; i++)
				ar>>vertices[i].x>>vertices[i].y>>vertices[i].z;
			for(int i=0; i<3; i++)
				ar>>normals[i].x>>normals[i].y>>normals[i].z;
			ar>>normal.x>>normal.y>>normal.z;
			ar>>color.x>>color.y>>color.z;
			ar>>textureIndex[0]>>textureIndex[1]>>textureIndex[2];

			for(int i=0; i<3; i++)
				edges[i].Serialize(ar);

			ar>>neighborTriIndex[0]>>neighborTriIndex[1]>>neighborTriIndex[2];
			ar>>area;
			ar>>vaild;

		}

	}



	int verticesIndex[3];
	int edgesIndex[3];
	int normalsIndex[3];
	Vector3f vertices[3];
	Vector3f normals[3];
	Vector3f normal;
	Vector3f color;

	int textureIndex[3];

	Edge edges[3];					// edge 0:(1,2), 1:(2,0), 2:(0,1)

	int neighborTriIndex[3];

	float area;
	bool vaild;
};




#endif