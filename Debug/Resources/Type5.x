xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 28;
 0.30000;0.00500;-0.30000;,
 0.28978;0.00500;0.07765;,
 0.28978;-0.00500;0.07765;,
 0.30000;-0.00500;-0.30000;,
 0.25981;0.00500;0.15000;,
 0.25981;-0.00500;0.15000;,
 0.21213;0.00500;0.21213;,
 0.21213;-0.00500;0.21213;,
 0.15000;0.00500;0.25981;,
 0.15000;-0.00500;0.25981;,
 0.07765;0.00500;0.28978;,
 0.07765;-0.00500;0.28978;,
 0.00000;0.00500;0.30000;,
 0.00000;-0.00500;0.30000;,
 -0.07765;0.00500;0.28978;,
 -0.07765;-0.00500;0.28978;,
 -0.15000;0.00500;0.25981;,
 -0.15000;-0.00500;0.25981;,
 -0.21213;0.00500;0.21213;,
 -0.21213;-0.00500;0.21213;,
 -0.25981;0.00500;0.15000;,
 -0.25981;-0.00500;0.15000;,
 -0.28978;0.00500;0.07765;,
 -0.28978;-0.00500;0.07765;,
 -0.30000;0.00500;-0.30000;,
 -0.30000;-0.00500;-0.30000;,
 0.00000;0.00500;-0.30000;,
 0.00000;-0.00500;-0.30000;;
 
 38;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;8,10,11,9;,
 4;10,12,13,11;,
 4;12,14,15,13;,
 4;14,16,17,15;,
 4;16,18,19,17;,
 4;18,20,21,19;,
 4;20,22,23,21;,
 4;22,24,25,23;,
 3;26,1,0;,
 3;26,4,1;,
 3;26,6,4;,
 3;26,8,6;,
 3;26,10,8;,
 3;26,12,10;,
 3;26,14,12;,
 3;26,16,14;,
 3;26,18,16;,
 3;26,20,18;,
 3;26,22,20;,
 3;26,24,22;,
 3;27,3,2;,
 3;27,2,5;,
 3;27,5,7;,
 3;27,7,9;,
 3;27,9,11;,
 3;27,11,13;,
 3;27,13,15;,
 3;27,15,17;,
 3;27,17,19;,
 3;27,19,21;,
 3;27,21,23;,
 3;27,23,25;,
 4;26,27,25,24;,
 4;0,3,27,26;;
 
 MeshMaterialList {
  1;
  38;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   100.000000;
   1.000000;1.000000;1.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "Type5.dds";
   }
  }
 }
 MeshNormals {
  16;
  0.999634;0.000000;0.027058;,
  -0.999634;0.000000;0.027058;,
  0.978056;0.000000;0.208343;,
  0.866024;0.000000;0.500002;,
  0.707107;0.000000;0.707107;,
  0.500002;0.000000;0.866024;,
  0.258817;0.000000;0.965926;,
  0.000000;0.000000;1.000000;,
  -0.258817;0.000000;0.965926;,
  -0.500002;0.000000;0.866024;,
  -0.707107;0.000000;0.707107;,
  -0.866024;0.000000;0.500002;,
  -0.978056;0.000000;0.208343;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;-0.000000;,
  0.000000;0.000000;-1.000000;;
  38;
  4;0,2,2,0;,
  4;2,3,3,2;,
  4;3,4,4,3;,
  4;4,5,5,4;,
  4;5,6,6,5;,
  4;6,7,7,6;,
  4;7,8,8,7;,
  4;8,9,9,8;,
  4;9,10,10,9;,
  4;10,11,11,10;,
  4;11,12,12,11;,
  4;12,1,1,12;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;13,13,13;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  3;14,14,14;,
  4;15,15,15,15;,
  4;15,15,15,15;;
 }
 MeshTextureCoords {
  28;
  0.000000;0.000000;
  -0.017040;0.629410;
  -0.017040;0.629410;
  0.000000;0.000000;
  -0.066990;0.750000;
  -0.066990;0.750000;
  -0.146450;0.853550;
  -0.146450;0.853550;
  -0.250000;0.933010;
  -0.250000;0.933010;
  -0.370590;0.982960;
  -0.370590;0.982960;
  -0.500000;1.000000;
  -0.500000;1.000000;
  -0.629410;0.982960;
  -0.629410;0.982960;
  -0.750000;0.933010;
  -0.750000;0.933010;
  -0.853550;0.853550;
  -0.853550;0.853550;
  -0.933010;0.750000;
  -0.933010;0.750000;
  -0.982960;0.629410;
  -0.982960;0.629410;
  -1.000000;0.000000;
  -1.000000;0.000000;
  -0.500000;0.000000;
  -0.500000;0.000000;;
 }
}
