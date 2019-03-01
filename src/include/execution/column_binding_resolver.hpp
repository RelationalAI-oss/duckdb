//===----------------------------------------------------------------------===//
//                         DuckDB
//
// execution/column_binding_resolver.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb.hpp"
#include "planner/logical_operator.hpp"
#include "planner/logical_operator_visitor.hpp"
#include "parser/expression/bound_columnref_expression.hpp"

namespace duckdb {

//! The ColumnBindingResolver resolves ColumnBindings into base tables
//! (table_index, column_index) into physical indices into the DataChunks that
//! are used within the execution engine
class ColumnBindingResolver : public LogicalOperatorVisitor {
public:
	ColumnBindingResolver() {
	}

	//! We overide the VisitOperator method because we don't want to automatically visit children of all operators
	void VisitOperator(LogicalOperator &op) override;

	vector<BoundTable> GetBoundTables() { return bound_tables; }
protected:
	void Visit(LogicalAggregate &op);
	void Visit(LogicalAnyJoin &op);
	void Visit(LogicalComparisonJoin &op);
	void Visit(LogicalCreateIndex &op);
	void Visit(LogicalEmptyResult &op);
	void Visit(LogicalUnion &op);
	void Visit(LogicalExcept &op);
	void Visit(LogicalIntersect &op);
	void Visit(LogicalCrossProduct &op);
	void Visit(LogicalChunkGet &op);
	void Visit(LogicalGet &op);
	void Visit(LogicalProjection &op);
	void Visit(LogicalSubquery &op);
	void Visit(LogicalTableFunction &op);
	void Visit(LogicalWindow &op);

	using SQLNodeVisitor::Visit;
	void Visit(ColumnRefExpression &expr) override {
		throw Exception(
		    "ColumnRefExpression is not legal here, should have been converted to BoundColumnRefExpression already!");
	}
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	// void Visit(BoundSubqueryExpression &expr) override;

	vector<BoundTable> bound_tables;
private:
	void PushBinding(BoundTable binding);
	void BindTablesBinaryOp(LogicalOperator &op, bool append_right);
	//! Append a list of tables to the current set of bound tables
	void AppendTables(vector<BoundTable> &right_tables);

	void ResolveSubquery(LogicalOperator &op);
};
} // namespace duckdb
