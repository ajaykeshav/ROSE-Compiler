using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

// namespace AbstractSyntaxTreeTraversalNamespace
//  {

public class AbstractSyntaxTreeTraversal : CSharpSyntaxWalker
   {
     static int Tabs = 0;
  // NOTE: Make sure you invoke the base constructor with 
  // the correct SyntaxWalkerDepth. Otherwise VisitToken()
  // will never get run.
     public AbstractSyntaxTreeTraversal() : base(SyntaxWalkerDepth.Token)
        {
        }

     public override void Visit(SyntaxNode node)
        {
          Tabs++;

       // Tabs are too much.
       // var indents = new String('\t', Tabs);
          var indents = new String(' ',Tabs*5);

          Console.WriteLine(indents + node.Kind());

       // Console.WriteLine(indents + "Add: " + node.GetDebuggerDisplay());
          Console.WriteLine(indents + "Enum value: " + node.RawKind);
       // Console.WriteLine(indents + "Add: " + node.ToString());

       // Check for the syntax kind (only on what would be more abstract syntax tree nodes).
          switch (node.Kind())
             {
            // names & type-names
               case SyntaxKind.IdentifierName: // = 8616,
                    break;

               case SyntaxKind.QualifiedName: // = 8617,
                    Console.WriteLine("Sorry, not implemented! node.Kind = " + node.Kind());

                    QualifiedNameSyntax qualifiedNameNode = node as QualifiedNameSyntax;

                    Console.WriteLine("   --- qualifiedNameNode.Kind      = " + qualifiedNameNode.Kind());
                 // Console.WriteLine("   --- qualifiedNameNode.Qualifier = " + qualifiedNameNode.Qualifier());
                 // Console.WriteLine("   --- qualifiedNameNode.Name      = " + qualifiedNameNode.Name());
                 // Console.WriteLine("   --- qualifiedNameNode.Qualifier = " + qualifiedNameNode.Identifier);
                 // Console.WriteLine("   --- qualifiedNameNode.Name      = " + qualifiedNameNode.Name());
                 // Console.WriteLine("   --- qualifiedNameNode.Name      = " + qualifiedNameNode.GetUnqualifiedName);
                    Console.WriteLine("   --- qualifiedNameNode.Left  = " + qualifiedNameNode.Left);
                    Console.WriteLine("   --- qualifiedNameNode.Right = " + qualifiedNameNode.Right);

                    break;

               case SyntaxKind.GenericName: // = 8618,
               case SyntaxKind.TypeArgumentList: // = 8619,
               case SyntaxKind.AliasQualifiedName: // = 8620,
               case SyntaxKind.PredefinedType: // = 8621,
               case SyntaxKind.ArrayType: // = 8622,
               case SyntaxKind.ArrayRankSpecifier: // = 8623,
               case SyntaxKind.PointerType: // = 8624,
               case SyntaxKind.NullableType: // = 8625,
               case SyntaxKind.OmittedTypeArgument: // = 8626,
                    break;
 
            // expressions
               case SyntaxKind.ParenthesizedExpression: // = 8632,
               case SyntaxKind.ConditionalExpression: // = 8633,
               case SyntaxKind.InvocationExpression: // = 8634,
               case SyntaxKind.ElementAccessExpression: // = 8635,
               case SyntaxKind.ArgumentList: // = 8636,
               case SyntaxKind.BracketedArgumentList: // = 8637,
               case SyntaxKind.Argument: // = 8638,
               case SyntaxKind.NameColon: // = 8639,
               case SyntaxKind.CastExpression: // = 8640,
               case SyntaxKind.AnonymousMethodExpression: // = 8641,
               case SyntaxKind.SimpleLambdaExpression: // = 8642,
               case SyntaxKind.ParenthesizedLambdaExpression: // = 8643,
               case SyntaxKind.ObjectInitializerExpression: // = 8644,
               case SyntaxKind.CollectionInitializerExpression: // = 8645,
               case SyntaxKind.ArrayInitializerExpression: // = 8646,
               case SyntaxKind.AnonymousObjectMemberDeclarator: // = 8647,
               case SyntaxKind.ComplexElementInitializerExpression: // = 8648,
               case SyntaxKind.ObjectCreationExpression: // = 8649,
               case SyntaxKind.AnonymousObjectCreationExpression: // = 8650,
               case SyntaxKind.ArrayCreationExpression: // = 8651,
               case SyntaxKind.ImplicitArrayCreationExpression: // = 8652,
               case SyntaxKind.StackAllocArrayCreationExpression: // = 8653,
               case SyntaxKind.OmittedArraySizeExpression: // = 8654,
               case SyntaxKind.InterpolatedStringExpression: // = 8655,
               case SyntaxKind.ImplicitElementAccess: // = 8656,
               case SyntaxKind.IsPatternExpression: // = 8657,
                    break;
 
            // binary expressions
               case SyntaxKind.AddExpression: // = 8668,
               case SyntaxKind.SubtractExpression: // = 8669,
               case SyntaxKind.MultiplyExpression: // = 8670,
               case SyntaxKind.DivideExpression: // = 8671,
               case SyntaxKind.ModuloExpression: // = 8672,
               case SyntaxKind.LeftShiftExpression: // = 8673,
               case SyntaxKind.RightShiftExpression: // = 8674,
               case SyntaxKind.LogicalOrExpression: // = 8675,
               case SyntaxKind.LogicalAndExpression: // = 8676,
               case SyntaxKind.BitwiseOrExpression: // = 8677,
               case SyntaxKind.BitwiseAndExpression: // = 8678,
               case SyntaxKind.ExclusiveOrExpression: // = 8679,
               case SyntaxKind.EqualsExpression: // = 8680,
               case SyntaxKind.NotEqualsExpression: // = 8681,
               case SyntaxKind.LessThanExpression: // = 8682,
               case SyntaxKind.LessThanOrEqualExpression: // = 8683,
               case SyntaxKind.GreaterThanExpression: // = 8684,
               case SyntaxKind.GreaterThanOrEqualExpression: // = 8685,
               case SyntaxKind.IsExpression: // = 8686,
               case SyntaxKind.AsExpression: // = 8687,
               case SyntaxKind.CoalesceExpression: // = 8688,
               case SyntaxKind.SimpleMemberAccessExpression: // = 8689,  // dot access:   a.b
               case SyntaxKind.PointerMemberAccessExpression: // = 8690,  // arrow access:   a->b
               case SyntaxKind.ConditionalAccessExpression: // = 8691,    // question mark access:   a?.b , a?[1]
                    break;
 
            // binding expressions
               case SyntaxKind.MemberBindingExpression: // = 8707,
               case SyntaxKind.ElementBindingExpression: // = 8708,
                    break;
 
            // binary assignment expressions
               case SyntaxKind.SimpleAssignmentExpression: // = 8714,
               case SyntaxKind.AddAssignmentExpression: // = 8715,
               case SyntaxKind.SubtractAssignmentExpression: // = 8716,
               case SyntaxKind.MultiplyAssignmentExpression: // = 8717,
               case SyntaxKind.DivideAssignmentExpression: // = 8718,
               case SyntaxKind.ModuloAssignmentExpression: // = 8719,
               case SyntaxKind.AndAssignmentExpression: // = 8720,
               case SyntaxKind.ExclusiveOrAssignmentExpression: // = 8721,
               case SyntaxKind.OrAssignmentExpression: // = 8722,
               case SyntaxKind.LeftShiftAssignmentExpression: // = 8723,
               case SyntaxKind.RightShiftAssignmentExpression: // = 8724,
                    break;
 
            // unary expressions
               case SyntaxKind.UnaryPlusExpression: // = 8730,
               case SyntaxKind.UnaryMinusExpression: // = 8731,
               case SyntaxKind.BitwiseNotExpression: // = 8732,
               case SyntaxKind.LogicalNotExpression: // = 8733,
               case SyntaxKind.PreIncrementExpression: // = 8734,
               case SyntaxKind.PreDecrementExpression: // = 8735,
               case SyntaxKind.PointerIndirectionExpression: // = 8736,
               case SyntaxKind.AddressOfExpression: // = 8737,
               case SyntaxKind.PostIncrementExpression: // = 8738,
               case SyntaxKind.PostDecrementExpression: // = 8739,
               case SyntaxKind.AwaitExpression: // = 8740,
                    break;
 
            // primary expression
               case SyntaxKind.ThisExpression: // = 8746,
               case SyntaxKind.BaseExpression: // = 8747,
               case SyntaxKind.ArgListExpression: // = 8748,
               case SyntaxKind.NumericLiteralExpression: // = 8749,
               case SyntaxKind.StringLiteralExpression: // = 8750,
               case SyntaxKind.CharacterLiteralExpression: // = 8751,
               case SyntaxKind.TrueLiteralExpression: // = 8752,
               case SyntaxKind.FalseLiteralExpression: // = 8753,
               case SyntaxKind.NullLiteralExpression: // = 8754,
               case SyntaxKind.DefaultLiteralExpression: // = 8755,
                    break;
 
            // primary function expressions
               case SyntaxKind.TypeOfExpression: // = 8760,
               case SyntaxKind.SizeOfExpression: // = 8761,
               case SyntaxKind.CheckedExpression: // = 8762,
               case SyntaxKind.UncheckedExpression: // = 8763,
               case SyntaxKind.DefaultExpression: // = 8764,
               case SyntaxKind.MakeRefExpression: // = 8765,
               case SyntaxKind.RefValueExpression: // = 8766,
               case SyntaxKind.RefTypeExpression: // = 8767,
                    break;

            // NameOfExpression: // = 8768, // we represent nameof(x) as an invocation expression
 
            // query expressions
               case SyntaxKind.QueryExpression: // = 8774,
               case SyntaxKind.QueryBody: // = 8775,
               case SyntaxKind.FromClause: // = 8776,
               case SyntaxKind.LetClause: // = 8777,
               case SyntaxKind.JoinClause: // = 8778,
               case SyntaxKind.JoinIntoClause: // = 8779,
               case SyntaxKind.WhereClause: // = 8780,
               case SyntaxKind.OrderByClause: // = 8781,
               case SyntaxKind.AscendingOrdering: // = 8782,
               case SyntaxKind.DescendingOrdering: // = 8783,
               case SyntaxKind.SelectClause: // = 8784,
               case SyntaxKind.GroupClause: // = 8785,
               case SyntaxKind.QueryContinuation: // = 8786,
                    break;
 
            // statements
               case SyntaxKind.Block: // = 8792,
               case SyntaxKind.LocalDeclarationStatement: // = 8793,
               case SyntaxKind.VariableDeclaration: // = 8794,
               case SyntaxKind.VariableDeclarator: // = 8795,
               case SyntaxKind.EqualsValueClause: // = 8796,
               case SyntaxKind.ExpressionStatement: // = 8797,
               case SyntaxKind.EmptyStatement: // = 8798,
               case SyntaxKind.LabeledStatement: // = 8799,
                    break;
 
            // jump statements
               case SyntaxKind.GotoStatement: // = 8800,
               case SyntaxKind.GotoCaseStatement: // = 8801,
               case SyntaxKind.GotoDefaultStatement: // = 8802,
               case SyntaxKind.BreakStatement: // = 8803,
               case SyntaxKind.ContinueStatement: // = 8804,
               case SyntaxKind.ReturnStatement: // = 8805,
               case SyntaxKind.YieldReturnStatement: // = 8806,
               case SyntaxKind.YieldBreakStatement: // = 8807,
               case SyntaxKind.ThrowStatement: // = 8808,
                    break;

               case SyntaxKind.WhileStatement: // = 8809,
               case SyntaxKind.DoStatement: // = 8810,
               case SyntaxKind.ForStatement: // = 8811,
               case SyntaxKind.ForEachStatement: // = 8812,
                    break;

               case SyntaxKind.UsingStatement: // = 8813,
                    Console.WriteLine("Sorry, not implemented! node.Kind = " + node.Kind());
                    break;

               case SyntaxKind.FixedStatement: // = 8814,
                    break;
 
            // checked statements
               case SyntaxKind.CheckedStatement: // = 8815,
               case SyntaxKind.UncheckedStatement: // = 8816,
                    break;
 
               case SyntaxKind.UnsafeStatement: // = 8817,
               case SyntaxKind.LockStatement: // = 8818,
               case SyntaxKind.IfStatement: // = 8819,
               case SyntaxKind.ElseClause: // = 8820,
               case SyntaxKind.SwitchStatement: // = 8821,
               case SyntaxKind.SwitchSection: // = 8822,
               case SyntaxKind.CaseSwitchLabel: // = 8823,
               case SyntaxKind.DefaultSwitchLabel: // = 8824,
               case SyntaxKind.TryStatement: // = 8825,
               case SyntaxKind.CatchClause: // = 8826,
               case SyntaxKind.CatchDeclaration: // = 8827,
               case SyntaxKind.CatchFilterClause: // = 8828,
               case SyntaxKind.FinallyClause: // = 8829,
                    break;
 
            // statements that didn't fit above
               case SyntaxKind.LocalFunctionStatement: // = 8830,
                    break;
 
            // declarations
               case SyntaxKind.CompilationUnit: // = 8840,
               case SyntaxKind.GlobalStatement: // = 8841,
               case SyntaxKind.NamespaceDeclaration: // = 8842,
                    break;

               case SyntaxKind.UsingDirective: // = 8843,
                    Console.WriteLine("Sorry, not implemented! node.Kind = " + node.Kind());

                    UsingDirectiveSyntax usingDirectiveNode = node as UsingDirectiveSyntax;

                    Console.WriteLine("   --- usingDirectiveNode.Kind = " + usingDirectiveNode.Kind());
                    Console.WriteLine("   --- usingDirectiveNode.Name = " + usingDirectiveNode.Name);

                    break;

               case SyntaxKind.ExternAliasDirective: // = 8844,
                    break;
 
            // attributes
               case SyntaxKind.AttributeList: // = 8847,
               case SyntaxKind.AttributeTargetSpecifier: // = 8848,
               case SyntaxKind.Attribute: // = 8849,
               case SyntaxKind.AttributeArgumentList: // = 8850,
               case SyntaxKind.AttributeArgument: // = 8851,
               case SyntaxKind.NameEquals: // = 8852,
                    break;
 
            // type declarations
               case SyntaxKind.ClassDeclaration: // = 8855,
               case SyntaxKind.StructDeclaration: // = 8856,
               case SyntaxKind.InterfaceDeclaration: // = 8857,
               case SyntaxKind.EnumDeclaration: // = 8858,
               case SyntaxKind.DelegateDeclaration: // = 8859,
                    break;
 
               case SyntaxKind.BaseList: // = 8864,
               case SyntaxKind.SimpleBaseType: // = 8865,
               case SyntaxKind.TypeParameterConstraintClause: // = 8866,
               case SyntaxKind.ConstructorConstraint: // = 8867,
               case SyntaxKind.ClassConstraint: // = 8868,
               case SyntaxKind.StructConstraint: // = 8869,
               case SyntaxKind.TypeConstraint: // = 8870,
               case SyntaxKind.ExplicitInterfaceSpecifier: // = 8871,
               case SyntaxKind.EnumMemberDeclaration: // = 8872,
               case SyntaxKind.FieldDeclaration: // = 8873,
               case SyntaxKind.EventFieldDeclaration: // = 8874,
               case SyntaxKind.MethodDeclaration: // = 8875,
               case SyntaxKind.OperatorDeclaration: // = 8876,
               case SyntaxKind.ConversionOperatorDeclaration: // = 8877,
               case SyntaxKind.ConstructorDeclaration: // = 8878,
                    break;
 
               case SyntaxKind.BaseConstructorInitializer: // = 8889,
               case SyntaxKind.ThisConstructorInitializer: // = 8890,
               case SyntaxKind.DestructorDeclaration: // = 8891,
               case SyntaxKind.PropertyDeclaration: // = 8892,
               case SyntaxKind.EventDeclaration: // = 8893,
               case SyntaxKind.IndexerDeclaration: // = 8894,
               case SyntaxKind.AccessorList: // = 8895,
               case SyntaxKind.GetAccessorDeclaration: // = 8896,
               case SyntaxKind.SetAccessorDeclaration: // = 8897,
               case SyntaxKind.AddAccessorDeclaration: // = 8898,
               case SyntaxKind.RemoveAccessorDeclaration: // = 8899,
               case SyntaxKind.UnknownAccessorDeclaration: // = 8900,
               case SyntaxKind.ParameterList: // = 8906,
               case SyntaxKind.BracketedParameterList: // = 8907,
               case SyntaxKind.Parameter: // = 8908,
               case SyntaxKind.TypeParameterList: // = 8909,
               case SyntaxKind.TypeParameter: // = 8910,
               case SyntaxKind.IncompleteMember: // = 8916,
               case SyntaxKind.ArrowExpressionClause: // = 8917,
                    break;

               default:
                    Console.WriteLine("Error: default reached in switch over node.Kind = " + node.Kind());
                    break;
             }

          base.Visit(node);
          Tabs--;
        }

     public override void VisitToken(SyntaxToken token)
        {
       // var indents = new String('\t', Tabs);
       // Console.WriteLine(indents + token);
       // base.VisitToken(token);
        }
   }

// }

