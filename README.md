# libezdxf

This is my C++ learning project - not for production usage!

The goal is a simple to use C++ interface to read and write DXF like my Python
[ezdxf](https://github.com/mozman/ezdxf) package.

## Design Goals

- The C++ version of `ezdxf` will focus on the file format interface!
  See the "Not Supported" section!

- The lib does not support concurrency beyond loading multiple DXF documents,
  which means loading one DXF document per thread. 
  Do no process a DXF document in multiple threads! 
  Providing thread safety is a lot of work without much benefit.

### 1st Stage

- Read/Write ASCII/Binary DXF

- Recover simple errors in DXF files and structures (behave more like BricsCAD, 
  don't be as mean and picky as A...C..) - but not as sophisticated as the 
  recover mode of the `ezdxf` Python package.
  
- Preserve 3rd party data in AppData, XData and Custom entities as raw DXF tags.
  
- Add new DXF entities, blocks and layouts.

- Edit entities and resources.

- Delete graphical DXF entities from blocks and layouts.

- Audit DXF entities, objects, resources.

- *Expert* mode for low level features:
  - Has to be activated deliberately: `doc.active_expert_mode(magic);`,
    the magic number is required to ensure that lib users have read the
    documentation or at least looked at the source code and saw the
    warnings.
  - Expert mode allows deleting resources -- everything which is referenced
    by other entities. But deleting such entities can do more harm than good!
    This feature requires full resource tracking which is not possible for
    a file format that is not fully documented and supports custom data
    and entities.

### 2nd Stage

- Transform DXF entities.

- Explode block references. 

- DWG Loader - the DWG reader will convert DWG data into DXF data and can only
  support entities documented by the Open Design Alliance (ODA).
  
- group_by()? Template function (static defined keys!) and a lambda function as 
  filter which returns the group key?
  
- Entity query language? This feature is not that easy to implement as in 
  Python - dynamic attribute access requires as massive amount of design and 
  implementation work (ANTLR), which is maybe not worth the effort. STL conform 
  iterators and filter functions in conjunction with lambdas, are maybe the 
  better choice.

## Not Supported - Not Planned

- No DWG exporter - writing the DWG format is too complex and DXF files can
  contain every feature of DWG.
  
- No support for creating BLOCK renderings for entities like DIMENSION and 
  ACAD_TABLE.
  At least not as primary goal.
  
- No rendering/explode support for LEADER, MLEADER.
  At least not as primary goal.
  
- Add-ons are not planned like for the Python `ezdxf` package! 
  No `Importer` or `drawing` add-ons!

- No support for importing or merging DXF files.

- No support to detect extents, limits or bounding boxes, because this 
  requires a full rendering engine.
  