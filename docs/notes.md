# Data Types and Units

## Handles

Handles are stored as `uint64_t`.
Handles are **maybe** database keys, see section [Data Model](#data-model).
The handle `0` is a special case and not a valid database key.

## Angles

Angles will be stored as radians the native angle unit for computers.
The DXF format store most angles as decimal degrees and some angles as radians,
the C++ implementation of `ezdxf` will unify this confusing behavior by always 
using radians (AutoLisp also uses only radians).

## Colors

### ACI

The Autocad Color Index is stored as an unsigned integer.

### RGB Channels

True color values are stored as floats in the range from `0.0` to `1.0` for each 
color channel.

### Alpha Channel 

The alpha channel is represented by a float following the [CSS definition][1] 
where `0.0` is fully transparent and `1.0` is fully opaque.

## Resources

All resources like layers, text styles, dimension styles, block references and 
so on will be stored as handles to the table entries like in the DWG format. 
This requires a resolving process because the DXF format stores many resources 
by name as case insensitive strings. DXF allows some resources without existing 
table entries (e.g. layers), which has to be fixed by automatically creating 
default table entries for such resources.

### Resource Tables

- Text style (STYLE): no resource references
- Shape file (STYLE): no resource references. This is technically a text style 
  but without a name -- I will not replicate this crappy design and therefore 
  shape files get a separated resource table
- Line type (LTYPE): may contain resource references by name
- Layer (LAYER): references line types by name, other resources are referenced 
  by handles
- Dimension style: R12 references resources by name, later DXF formats references 
  resources by handles.
- Application identifier (APPID): no resource references
- UCS: no resource references
- Views (VIEW): resources referenced by handles
- Viewports (VPORT): some resources are referenced by name. A viewport 
  configuration consist of one or more VPORT entries.  
- Block record (BLOCK_RECORD): associated BLOCK is referenced by name, LAYOUT is
  referenced by handle

### Resource Objects

Objects were introduced in the DXF R13+ format and they use always (?) handles 
to reference resources.

- Material
- Plot style
- Layout definition
- MLine style
- MLeader style
- Visual style
- Table style
- ImageDef
- UnderlayDef

## Data Model

This section describes how the lib stores and manages DXF entities and objects 
internally.

The ezdxf library is meant to be used as interface to the DXF format. 
The common usage is expected as:

- **LOAD** DXF document: load - read data - close/discard
  - create: loading speed of the document is not important (I/O bound)
  - add: -  
  - read: random and linear access should be fast
  - delete: removing entities is not important, can be slow
  - export: -
  - close: without leaks, speed is secondary
  
- **EDIT** DXF Document: load - add/edit/remove data - export - close/discard
  - create: loading speed of the document is not important (I/O bound)
  - add: creating and storing entities should be fast
  - read: random and linear access should be fast
  - delete: removing entities should be fast
  - export: error free export is important, speed is not important (I/O bound)
  - close: without leaks, speed is secondary
  
- **NEW** DXF Document: new - add data - export - close/discard
  - create: speed of document setup is not important
  - add: creating and storing entities at runtime should be fast
  - read: random or linear access speed is not important
  - delete: -
  - export: error free export is important, speed is not important (I/O bound)
  - close: without leaks, speed is secondary

"Not important" means: as fast as possible without adding complexity!

The ezdxf library is **not** meant to represent the data storage for 
applications!

The entity count is not impressive for C/C++. I assume not more than 100.000 
entities per DXF document and this is already a huge DXF drawing, which are not 
very usable in CAD applications.

### EntityDB

The entity database is the main storage of the DXF entities. The DB owns the 
entities from the point the entity was added and will never transfer this 
ownership until the destruction of the entity database which also destroys the 
entity.

The database has to manage reserved handles to entities which will not 
represented as a real DXF entity: Table Head, SEQEND, ENDBLK, (VERTEX?)  

#### Data Structure

Like in the Python version of ezdxf, the handle as hash key into an unordered 
map (like Python dict) seems to be the right choice. But Alan Talbot mentioned 
in the last part of his video[2] at the CppCon 2018, that unordered maps have 
a node based implementation in the STL and are not very fast or memory 
efficient and `vector` is in most cases the fastest container.

By expecting less than 100.000 entities per DXF document seems a `vector` based 
implementation reasonable.

**CREATE**: Entities will be added to the DB only a the end of the vector 
and is therefore very fast, beside reallocation -- reserve a fair amount at 
setup.

**READ**: By using the DBIndex as internal handle, linear and random read 
is very fast. 

But the DXF handle to DBIndex mapping requires an additional data 
structure -- if access by DXF handle is not very often required (DBIndex as 
internal handle) a linear search is maybe sufficient.

**DELETE**: The DBIndex of entities has to be immutable. 
The entity will be marked as deleted but not destroyed. 
Not destroying entities during the lifetime of the DXF document also prevent 
invalid entity references (dangling pointers). 

Referencing entries by raw pointers is reliable, as the entities are managed 
by the database and entities can not be destroyed during the lifetime of the 
DXF document.

**Never** reassign a freed DXF handle to a new DXF entity! 
DXF supports 2^64 handles, recycling of handles should not be necessary.

The relationship **DXFHandle/DBIndex/entity pointer is immutable**, therefore 
container could also store the entity pointer instead of the DBIndex.

Using an `unique_ptr` to store entities in the database shows clearly who owns 
the entity!

### Entities/Objects

**Entities are not destroyed during the lifetime of a DXF document!**

The entity requires a flag to store this status (and maybe more flags later).
A call to `DXFEntity.is_alive()` is **always** valid. Therefore storing 
references to entities as **raw** pointers is reliable, fast and simple.

At some time the raw pointer to a DXF entity has to be exposed to the lib 
users. Lib users have to agree to the policy: 
"You don't own it, don't delete it!" ?

### Containers

By using the DBIndex as internal handle, all resource tables and entity spaces 
can use a simple `vector<int>` as entity storage.

It's faster and also reliable to store reference pointers to entities, 
because the relationship **DXFHandle/DBIndex/entity pointer** is immutable 
and entities will not be destroyed during the lifetime of the DXF document.

[1]: https://developer.mozilla.org/en-US/docs/Web/CSS/alpha-value
[2]: https://www.youtube.com/watch?v=EovBkh9wDnM