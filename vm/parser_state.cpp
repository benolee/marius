#include "parser_state.hpp"
#include <assert.h>

namespace r5 {

  String* ParserState::string(const char* s) {
    return String::internalize(S, s);
  }

  void ParserState::set_top(ast::Node* b) {
    ast::Argument* a = new ast::Argument(String::internalize(S, "self"), -1);

    top_ = pos(new ast::Scope(b, context_->local_names, a));
  }

  ast::Node* ParserState::seq(ast::Node* l, ast::Node* r) {
    return pos(new ast::Seq(l, r), l);
  }

  ast::Node* ParserState::ast_class(String* name, ast::Node* super,
                                    ast::Node* body)
  {
    ast::Argument* a = new ast::Argument(String::internalize(S, "self"), -1);

    if(!super) {
      super = named(String::internalize(S, "Object"), parser_.line());
    }

    ast::Node* n = pos(new ast::Class(name, super,
                         new ast::Scope(body, context_->local_names, a)));

    delete context_;

    context_ = stack_.back();
    stack_.pop_back();

    return n;
  }

  ast::Node* ParserState::trait(String* name, ast::Node* body) {
    ast::Argument* a = new ast::Argument(String::internalize(S, "self"), -1);

    ast::Node* n = pos(new ast::Trait(name,
                         new ast::Scope(body, context_->local_names, a)));

    delete context_;

    context_ = stack_.back();
    stack_.pop_back();

    return n;
  }

  void ParserState::start_def() {
    stack_.push_back(context_);

    context_ = new Context();
  }

  ast::Node* ParserState::ast_def(String* name, ast::Node* b) {
    ast::Argument* a = new ast::Argument(String::internalize(S, "self"), -1);

    ast::Node* n = pos(new ast::Def(0, name, 
                         new ast::Scope(b, context_->local_names,
                            context_->args, context_->arg_objs, a),
                         context_->args));
    delete context_;

    context_ = stack_.back();
    stack_.pop_back();

    return n;
  }

  ast::Node* ParserState::ast_def_spec(String* scope, String* name,
                                       ast::Node* b)
  {
    ast::Argument* a = new ast::Argument(String::internalize(S, "self"), -1);

    ast::Node* n = pos(new ast::Def(scope, name, 
                         new ast::Scope(b, context_->local_names,
                            context_->args, context_->arg_objs, a),
                         context_->args));
    delete context_;

    context_ = stack_.back();
    stack_.pop_back();

    return n;
  }

  void ParserState::def_arg(String* name) {
    int num = context_->args.size();

    context_->args[name] = num;
    context_->arg_objs.push_back(new ast::Argument(name, num));
  }

  void ParserState::def_opt_arg(String* name, ast::Node* val) {
    int num = context_->args.size();

    context_->args[name] = num;
    context_->arg_objs.push_back(new ast::Argument(name, 0, num, val));
  }

  void ParserState::def_arg_cast(String* name, ast::Node* c) {
    int num = context_->args.size();

    context_->args[name] = num;
    context_->arg_objs.push_back(new ast::Argument(name, c, num, 0));
  }

  ast::Node* ParserState::send(ast::Node* recv, String* n) {
    return pos(new ast::Send(n, recv), recv);
  }

  ast::Node* ParserState::send_attr(ast::Node* recv, String* n) {
    return pos(new ast::Send(n, recv, 0, ast::Send::eAttr), recv);
  }

  ast::Node* ParserState::send_set_attr(ast::Node* recv, String* n,
                                        ast::Node* val)
  {
    ast::Nodes nodes;
    nodes.push_back(val);

    ast::Arguments* args = new ast::Arguments(nodes, ArgMap());

    return pos(new ast::Send(n, recv, args, ast::Send::eSetAttr), recv);
  }

  ast::Node* ParserState::self_send(String* n) {
    return pos(new ast::Send(n, self(), 0, ast::Send::eSelfLess));
  }

  ast::Node* ParserState::send_indirect(ast::Node* recv, ast::Node* n) {
    return pos(new ast::SendIndirect(n, recv));
  }

  ast::Node* ParserState::send_indirect_args(ast::Node* recv, ast::Node* name) {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);

    n = pos(new ast::SendIndirect(name, recv, args));

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    return n;
  }

  ast::Node* ParserState::dcolon(ast::Node* recv, String* n, String* a) {
    return pos(new ast::Send(n, recv, ast::Arguments::wrap(new ast::LiteralString(a))), recv);
  }

  ast::Node* ParserState::lit_str(String* n) {
    return pos(new ast::LiteralString(n));
  }

  ast::Node* ParserState::attr(ast::Node* r, String* n) {
    return pos(new ast::LoadAttr(r, n));
  }

  ast::Node* ParserState::named(String* s, int line) {
    return pos(new ast::Named(s), line);
  }

  void ParserState::start_class() {
    stack_.push_back(context_);

    context_ = new Context();
  }

  void ParserState::start_trait() {
    stack_.push_back(context_);

    context_ = new Context();
  }

  ast::Send* ParserState::ast_send(String* name, ast::Node* r, ast::Nodes args) {
    return pos(new ast::Send(name, r, new ast::Arguments(args)), r);
  }

  ast::Send* ParserState::ast_binop(const char* s, ast::Node* a, ast::Node* b) {
    return pos(new ast::Send(String::internalize(S, s), a, ast::Arguments::wrap(b)), a);
  }

  ast::Send* ParserState::ast_binop(String* op, ast::Node* a, ast::Node* b) {
    return pos(new ast::Send(op, a, ast::Arguments::wrap(b)), a);
  }

  ast::Node* ParserState::number(int a) {
    return pos(new ast::Number(a));
  }

  ast::Node* ParserState::ret(ast::Node* n) {
    return pos(new ast::Return(n));
  }

  void ParserState::start_cascade(ast::Node* recv) {
    cascades_.push_back(new ast::Cascade(recv));
  }

  void ParserState::cascade(String* name) {
    cascades_.back()->push_message(new ast::CascadeSend(name));
  }

  ast::Node* ParserState::end_cascade() {
    ast::Node* n = cascades_.back();
    cascades_.pop_back();

    return n;
  }

  void ParserState::start_arg_list() {
    arg_infos_.push_back(arg_info_);

    arg_info_ = ArgInfo();
  }

  void ParserState::add_arg(ast::Node* a) {
    arg_info_.nodes.push_back(a);
  }

  void ParserState::add_kw_arg(String* name, ast::Node* a) {
    arg_info_.keywords[name] = arg_info_.nodes.size();
    arg_info_.nodes.push_back(a);
  }

  ast::Node* ParserState::send_args(ast::Node* recv, String* id) {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);

    n = pos(new ast::Send(id, recv, args), recv);

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    assert(n);

    return n;
  }

  ast::Node* ParserState::self_send_args(String* id) {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);

    n = pos(new ast::Send(id, self(), args, ast::Send::eSelfLess));

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    assert(n);

    return n;
  }

  ast::Node* ParserState::send_args(ast::Node* recv, const char* id) {
    return send_args(recv, String::internalize(S, id));
  }

  ast::Node* ParserState::tuple() {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);

    n = pos(new ast::Tuple(args));

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    assert(n);

    return n;
  }

  ast::Node* ParserState::list() {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);

    n = pos(new ast::List(args));

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    assert(n);

    return n;
  }

  ast::Node* ParserState::dict() {
    ast::Node* n = 0;

    ast::Arguments* args = new ast::Arguments(arg_info_.nodes, arg_info_.keywords);
    n = pos(new ast::Dictionary(args));

    arg_info_ = arg_infos_.back();
    arg_infos_.pop_back();

    assert(n);

    return n;
  }

  ast::Node* ParserState::if_cond(ast::Node* cond, ast::Node* body) {
    return pos(new ast::IfCond(cond, body));
  }

  ast::Node* ParserState::if_else(ast::Node* cond, ast::Node* body,
                                  ast::Node* ebody)
  {
    return pos(new ast::IfCond(cond, body, ebody));
  }

  ast::Node* ParserState::unless(ast::Node* cond, ast::Node* body) {
    return pos(new ast::Unless(cond, body));
  }

  ast::Node* ParserState::while_(ast::Node* cond, ast::Node* body) {
    return pos(new ast::While(cond, body));
  }

  ast::Node* ParserState::ast_nil() {
    return pos(new ast::Nil());
  }

  ast::Node* ParserState::ast_true() {
    return pos(new ast::True());
  }

  ast::Node* ParserState::ast_false() {
    return pos(new ast::False());
  }

  ast::Node* ParserState::self() {
    return pos(new ast::Self());
  }

  ast::Node* ParserState::import(String* name) {
    return pos(new ast::Import(S, name));
  }

  ast::Node* ParserState::import(String* path, String* name) {
    return pos(new ast::Import(S, path, name));
  }

  ast::Node* ParserState::import_one(String* name, String* elem) {
    return pos(new ast::ImportOne(S, name, elem));
  }

  ast::Node* ParserState::import_one(String* path, String* elem, String* name) {
    return pos(new ast::ImportOne(S, path, elem, name));
  }

  ast::Node* ParserState::ast_try(ast::Node* b, ast::Node* h) {
    return pos(new ast::Try(b, h));
  }

  ast::Node* ParserState::ast_try(ast::Node* b, String* id, ast::Node* t,
                                  ast::Node* h)
  {
    return pos(new ast::Try(b, h, id, t));
  }

  ast::Node* ParserState::assign(String* name, ast::Node* n, int line) {
    return pos(new ast::Assign(name, n), line);
  }

  ast::Node* ParserState::ivar_assign(String* name, ast::Node* v) {
    return pos(new ast::IvarAssign(name, v), v);
  }

  ast::Node* ParserState::assign_op(String* name, String* op, ast::Node* n) {
    return pos(new ast::AssignOp(name, op, n), n);
  }

  ast::Node* ParserState::ivar_assign_op(String* name, String* op, ast::Node* n) {
    return pos(new ast::IvarAssignOp(name, op, n));
  }

  ast::Node* ParserState::ivar_read(String* name) {
    return pos(new ast::IvarRead(name));
  }

  void ParserState::start_lambda() {
    stack_.push_back(context_);

    context_ = new Context();
  }

  ast::Node* ParserState::lambda(ast::Node* b) {
    ast::Scope* sc = new ast::Scope(b, context_->local_names,
                        context_->args, context_->arg_objs, 0);

    ast::Node* n = pos(new ast::Lambda(sc));

    delete context_;

    context_ = stack_.back();
    stack_.pop_back();

    return n;
  }

  ast::Node* ParserState::cast(ast::Node* e, ast::Node* type) {
    return pos(new ast::Cast(e, type));
  }

  ast::Node* ParserState::raise(ast::Node* v) {
    return pos(new ast::Raise(v));
  }

  ast::Node* ParserState::not_(ast::Node* v) {
    return pos(new ast::Not(v));
  }

  ast::Node* ParserState::and_(ast::Node* a, ast::Node* b) {
    return pos(new ast::And(a, b));
  }
}
