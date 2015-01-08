#include "data/parser/cxx/ASTBodyVisitor.h"

ASTBodyVisitor::ASTBodyVisitor(ASTBodyVisitorClient* client, clang::FunctionDecl* functionDecl)
	: m_client(client)
	, m_functionDecl(functionDecl)
	, m_varDecl(nullptr)
{
}

ASTBodyVisitor::ASTBodyVisitor(ASTBodyVisitorClient* client, clang::VarDecl* varDecl)
	: m_client(client)
	, m_functionDecl(nullptr)
	, m_varDecl(varDecl)
{
}

ASTBodyVisitor::~ASTBodyVisitor()
{
}

void ASTBodyVisitor::VisitStmt(clang::Stmt* stmt)
{
	VisitChildren(stmt);
}

void ASTBodyVisitor::VisitChildren(clang::Stmt* stmt)
{
	for (clang::Stmt::child_range it = stmt->children(); it; it++)
	{
		if (*it)
		{
			static_cast<ASTBodyVisitor*>(this)->Visit(*it);
		}
	}
}

void ASTBodyVisitor::VisitCallExpr(clang::CallExpr* expr)
{
	if (m_functionDecl)
	{
		m_client->VisitCallExprInDeclBody(m_functionDecl, expr);
	}
	else
	{
		m_client->VisitCallExprInDeclBody(m_varDecl, expr);
	}

	VisitStmt(expr);
}

void ASTBodyVisitor::VisitCXXConstructExpr(clang::CXXConstructExpr* expr)
{
	if (m_functionDecl)
	{
		m_client->VisitCXXConstructExprInDeclBody(m_functionDecl, expr);
	}
	else
	{
		m_client->VisitCXXConstructExprInDeclBody(m_varDecl, expr);
	}

	VisitStmt(expr);
}

void ASTBodyVisitor::VisitCXXNewExpr(clang::CXXNewExpr* expr)
{
	if (m_functionDecl)
	{
		m_client->VisitCXXNewExprInDeclBody(m_functionDecl, expr);
	}
	else
	{
		m_client->VisitCXXNewExprInDeclBody(m_varDecl, expr);
	}

	VisitStmt(expr);
}

void ASTBodyVisitor::VisitMemberExpr(clang::make_ptr<clang::MemberExpr>::type expr)
{
	if (expr->getMemberDecl()->getKind() == clang::Decl::Kind::Field)
	{
		m_client->VisitMemberExprInDeclBody(m_functionDecl, expr);
	}
	VisitStmt(expr);
}

void ASTBodyVisitor::VisitDeclRefExpr(clang::make_ptr<clang::DeclRefExpr>::type expr)
{
	if (expr->getDecl()->getKind() == clang::Decl::Var && expr->getDecl()->isDefinedOutsideFunctionOrMethod())
	{
		m_client->VisitGlobalVariableExprInDeclBody(m_functionDecl, expr);
	}
	else if (expr->getDecl()->getKind() == clang::Decl::EnumConstant)
	{
		if (m_functionDecl)
		{
			m_client->VisitEnumExprInDeclBody(m_functionDecl, expr);
		}
		else
		{
			m_client->VisitEnumExprInDeclBody(m_varDecl, expr);
		}
	}
	VisitStmt(expr);
}

void ASTBodyVisitor::VisitDeclStmt(clang::DeclStmt* stmt)
{
	for (clang::Decl* decl : stmt->decls())
	{
		if (clang::isa<clang::VarDecl>(decl))
		{
			m_client->VisitVarDeclInDeclBody(m_functionDecl, clang::dyn_cast<clang::VarDecl>(decl));
		}
	}
	VisitStmt(stmt);
}
