//===----------------------------------------------------------------------===//
//                         DuckDB
//
// execution/physical_operator.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "catalog/catalog.hpp"
#include "common/common.hpp"
#include "common/printer.hpp"
#include "common/types/data_chunk.hpp"
#include "parser/expression.hpp"
#include "parser/statement/select_statement.hpp"
#include "planner/logical_operator.hpp"

namespace duckdb {
class ClientContext;
class ExpressionExecutor;
class PhysicalOperator;

//! The current state/context of the operator. The PhysicalOperatorState is
//! updated using the GetChunk function, and allows the caller to repeatedly
//! call the GetChunk function and get new batches of data everytime until the
//! data source is exhausted.
class PhysicalOperatorState {
public:
	PhysicalOperatorState(PhysicalOperator *child);
	virtual ~PhysicalOperatorState() {
	}

	//! Flag indicating whether or not the operator is finished [note: not all
	//! operators use this flag]
	bool finished;
	//! DataChunk that stores data from the child of this operator
	DataChunk child_chunk;
	//! State of the child of this operator
	unique_ptr<PhysicalOperatorState> child_state;
	//! The cached result of already-computed Common Subexpression results
	unordered_map<Expression *, unique_ptr<Vector>> cached_cse;
};

//! PhysicalOperator is the base class of the physical operators present in the
//! execution plan
/*!
    The execution model is a pull-based execution model. GetChunk is called on
   the root node, which causes the root node to be executed, and presumably call
   GetChunk again on its child nodes. Every node in the operator chain has a
   state that is updated as GetChunk is called: PhysicalOperatorState (different
   operators subclass this state and add different properties).
*/
class PhysicalOperator {
public:
	PhysicalOperator(PhysicalOperatorType type, vector<TypeId> types) : type(type), types(types) {
	}
	virtual ~PhysicalOperator(){}

	PhysicalOperatorType GetOperatorType() {
		return type;
	}

	string ToString(size_t depth = 0) const;
	void Print() {
		Printer::Print(ToString());
	}

	//! Return a vector of the types that will be returned by this operator
	vector<TypeId> &GetTypes() {
		return types;
	}
	//! Initialize a given chunk to the types that will be returned by this
	//! operator, this will prepare chunk for a call to GetChunk. This method
	//! only has to be called once for any amount of calls to GetChunk.
	virtual void InitializeChunk(DataChunk &chunk) {
		auto &types = GetTypes();
		chunk.Initialize(types);
	}
	//! Retrieves a chunk from this operator and stores it in the chunk
	//! variable.
	virtual void _GetChunk(ClientContext &context, DataChunk &chunk, PhysicalOperatorState *state) = 0;

	void GetChunk(ClientContext &context, DataChunk &chunk, PhysicalOperatorState *state);

	//! Create a new empty instance of the operator state
	virtual unique_ptr<PhysicalOperatorState> GetOperatorState() {
		return make_unique<PhysicalOperatorState>(children.size() == 0 ? nullptr : children[0].get());
	}

	//! will pass the visitor to all expressions in the operator
	virtual void AcceptExpressions(SQLNodeVisitor *v) = 0;

	void Accept(SQLNodeVisitor *v) {
		assert(v);
		AcceptExpressions(v);
		for (auto &child : children) {
			child->Accept(v);
		}
	}

	virtual string ExtraRenderInformation() {
		return "";
	}

	//! The physical operator type
	PhysicalOperatorType type;
	//! The set of children of the operator
	vector<unique_ptr<PhysicalOperator>> children;
	//! The types returned by this physical operator
	vector<TypeId> types;
};
} // namespace duckdb
