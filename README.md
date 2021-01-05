# libezdxf

This is my C++ learning project - not for production usage!

The goal is a simple to use C++ interface to read and write DXF like my Python
[ezdxf](https://github.com/mozman/ezdxf) package.

## Design Goals

- The C++ version of `ezdxf` will focus on the file format interface! 
  See the "Not Supported" section!

- Read ASCII/Binary DXF and DWG.

- The lib does not support concurrency beyond loading multiple DXF documents 
  -- each document in one separated thread. Do no process a DXF document in 
  multiple threads! Providing thread safety is a lot of work without much 
  benefit.   

- The DWG reader will convert DWG data into DXF data and can only 
  support entities documented by the Open Design Alliance(ODA).

- Write ASCII/Binary DXF

- No DWG exporter - writing the DWG format is too complex and DXF files can 
  contain every feature of DWG. 

- Recover simple errors in DXF files and structures (behave more like BricsCAD, 
  don't be as mean and picky as A...C..) - but not as sophisticated as the 
  recover mode of the `ezdxf` Python package.
  
- Preserve 3rd party data in AppData, XData and Custom entities as raw DXF tags.
  The DWG reader can only preserve documented entities!
  
- Add new DXF entities, blocks and layouts.

- Edit entities and resources.

- Delete graphical DXF entities from blocks and layouts.

- Audit DXF entities, objects, resources.

- Transform DXF entities.

- Explode block references.

- Entity query language.

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

## Not Supported - Not Planned

- No support to delete resources without activating the *expert* mode.
  
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
  