//===----------------------------------------------------------------------===//
//                         DuckDB
//
// parser/constraint.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/common.hpp"

namespace duckdb {

class Serializer;
class Deserializer;

class SQLNodeVisitor;
//! Constraint is the base class of any type of table constraint.
class Constraint {
public:
	Constraint(ConstraintType type) : type(type){};
	virtual ~Constraint() {
	}

	virtual void Accept(SQLNodeVisitor *) = 0;

	virtual string ToString() const = 0;
	void Print();
	
	ConstraintType type;

	unique_ptr<Constraint> Copy();
	//! Serializes a Constraint to a stand-alone binary blob
	virtual void Serialize(Serializer &serializer);
	//! Deserializes a blob back into a Constraint, returns NULL if
	//! deserialization is not possible
	static unique_ptr<Constraint> Deserialize(Deserializer &source);
};
} // namespace duckdb
