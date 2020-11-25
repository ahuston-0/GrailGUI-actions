#pragma once

#include <iostream>
#include <unordered_map>

#include "opengl/Errcode.hh"
#include "util/Buffer.hh"
#include "util/DynArray.hh"
#include "util/Ex.hh"
#include "util/datatype.hh"
#include "xdl/std.hh"

class XDLCompiler;

class SymbolTable : public Struct {
 private:
  uint32_t root;
 public:
  SymbolTable(XDLCompiler* c);

  void addRoot(const XDLType* t);

  // this is a hack, root happens to be = the number after all enums...
  const XDLType* getRoot() const { return members[root].type; }
  Struct* addStruct(const string& name);
  void addXDLType(const std::string& name, XDLType* xdlType);
  //void writeMeta(Buffer& metadataBuf) override;
  void write(Buffer& out);
  // read in metadata from buffer and return pointer to the type being added to
  // the symbol table
  void readMeta(Buffer& metadataBuf);
  // dump a specific type as text
  void displayText(Buffer& binaryIn, Buffer& asciiOut) const;
};
