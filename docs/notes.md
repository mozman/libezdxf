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

Simple hash-map entity database: 
Key is the entity handle & `0x3FF`, which creates 1024 different hash values. 
The DXF entities are stored as unique pointers in an unordered `std::vector`. 
Top level structure is another `std::vector` which stores an entity sub-list 
for each hash value. A simple linear search is used to find entities by handle 
in this sub-lists. 
This should be fast enough, access by handle will not be the main access method, 
references between various DXF entities will be stored as raw pointers.


### Entities/Objects

**Entities are not destroyed during the lifetime of a DXF document!**

The entity requires a flag to store this status (and maybe more flags later).
A call to `DXFEntity.is_alive()` is **always** valid. Therefore storing 
references to entities as **raw** pointers is reliable, fast and simple.

At some time the raw pointer to a DXF entity has to be exposed to the lib 
users. Lib users have to agree to the policy: 
"You don't own it, don't delete it!" ?

Every DXF entity/object reside in ONE container (owner), but can be 
referenced by multiple DXF entities/objects.

#### Creating DXF entities

1. Global factory function:

```C++
auto doc = ezdxf::new_doc(ezdxf::Version::R2010);
// Create a new LINE entity, returns an unique_ptr<ezdxf::entities::DXFEntity>
auto line = ezdxf::factory::create(ezdxf::ents::Line);
auto msp = doc.get_modelspace();
// Adding the entity to a layout moves the entity ownership 
// into the EntityDB:
// Returns a new reference as raw pointer to the Line object
auto pline = msp.add(line); 
// unique_ptr<...> line is now a nullptr

// Set DXF properties, see also 2. point:
auto props ezdxf::DXFProperties{doc};
// ...
pline->set_properties(props);

```

2. Factory functions of layouts, like in the Python version:

```C++
auto doc = ezdxf::new_doc(ezdxf::Version::R2010); // unique_ptr<ezdxf::Document>
auto msp = doc.get_modelspace();  // ezdxf::Layout*, just a reference
auto = doc.layers.create("MyLayer");  // ezdxf::entities::Layer*
my_layer->set_aci_color(2);
my_layer->set_lineweight(50);

// Create a reusable DXF properties object and get default values 
// from the DXF document like the default layer or text style:
auto props ezdxf::DXFProperties{doc};  
props.set_aci_color(1);
props.set_layer(my_layer);

// create new entities by factory methods: 
auto pline = msp.add_line({0, 0, 0}, {1, 0, 0}, props); 
auto pline = msp.add_line({2, 0, 0}, {3, 0, 0}, props);

```
   
3. Direct instantiation and copying:

```C++
auto line = ezdxf::entities::Line();
auto msp = doc.get_modelspace();
// Adding the entity to a layout as local object 
// creates a new entity in the EntityDB:
// Returns a reference as raw pointer to the Line() object
auto pline = msp.add(line); 
```

#### DXF Attribute Access

The static nature of C++ will prevent the dynamic features of the Python 
version like `get_dxf_attrib("lineweight")`, the return type would be a 
`std::variant` or `std::any` and access is not that "easy" as I would like.

So the DXF attribute will be a static approach, some ideas:

1. Simple direct solution
   
Store attributes as members of the entity class, access by getter/setter 
functions:
   
```C++
auto line = ezdxf::factory::create(ezdxf::ents::Line);
line->set_start_point({0, 0, 0});
line->set_end_point({1, 0, 0});
line->set_aci_color(1);
line->set_true_color({0.9, 0.1, 0.1});

auto msp = doc.get_modelspace();
auto pline = msp.add(line); 
// Get the new assigned handle:
unit64_t handle = pline->get_handle();  
// Get reference ptr to the parent layout:
auto owner = pline->get_owner();  
assert(owner == msp)

// Create a reusable DXF properties object and get default values 
// from the DXF document like the default layer or text style:
auto props ezdxf::DXFProperties{doc};
props.set_aci_color(1);
props.set_layer(my_layer);
pline->set_properties(props);

```

2. A DXF namespace object, like in the Python version:

```C++
auto line = ezdxf::factory::create(ezdxf::ents::Line);
line->dxf.start_point.set({0, 0, 0});
line->dxf.end_point.set({1, 0, 0});
line->dxf.aci_color.set(1);
line->dxf.true_color.set({0.9, 0.1, 0.1});

auto msp = doc.get_modelspace();
auto line_ptr = msp.add(line);

// getters:
auto handle = line_ptr->dxf.handle.get();  
auto owner = line_ptr->dxf.owner.get();

// getters could be nicer by overloading the 'operator()()' 
// of the DXF properties:
auto handle = line_ptr->dxf.handle();
auto owner = line_ptr->dxf.owner();
```


### Containers

All resource tables and entity spaces can use a simple `vector<DXFEntity*>` 
as entity storage.

It's reliable to store reference pointers to entities, 
because the relationship **DXFHandle/entity pointer** is immutable 
and entities will not be destroyed during the lifetime of the DXF document.

[1]: https://developer.mozilla.org/en-US/docs/Web/CSS/alpha-value
[2]: https://www.youtube.com/watch?v=EovBkh9wDnM