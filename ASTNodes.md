

# C* AST Nodes

### < Type > Nodes

- `<SingleTypeRef name="">`
- `<ComposeTypeRef name="" types=[Type]>`
- `<Lifetime ref="">`
- `<ReferenceType lifetime=Lifetime type="">`
- `<OptionalType type=Type>`
- `<VecType type=Type>`
- `<ArrayType type=Type>`

### < Expression > Nodes

- `<Integer value="">`
- `<Bool value="">`
- `<String value="">`
- `<Char value="">`
- `<Float value="">`
- `<Null>`
- `<Empty>`
- `<Range from=Expression step=Expression to=Expression>`


- `<Expression ADD Expression>`
- `<Expression SUB Expression>`
- `<Expression MUL Expression>`
- `<Expression DIV Expression>`
- `<Expression MOD Expression>`
- `<Expression POW Expression>`
- `<PREINC Expression>`
- `<POSTINC Expression>`
- `<PREDEC Expression>`
- `<POSTDEC Expression>`
- `<Expression CAST Type>`
- `<Expression IN Expression>`

- `<Varcall name="">`
- `<CALL name="" params=[Expression]>`
- `<INDEX name="" at=Expression>`

### < Statement > Nodes

- `{}`
- `Expression`
- `<IF Expression>{}`
- `<ELSE>{}`
- `<Vardec type=Type name="" value=Expression>`
- `<FOREACH type=Type name="" Expression>`
- `<FOR >`



