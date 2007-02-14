 /*
  Copyright (C) 2004, 2005 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSER_STACK_H
#define MU_PARSER_STACK_H

#include <cassert>
#include <string>
#include <stack>
#include <vector>

#include "muParserError.h"
#include "muParserToken.h"


namespace mu
{

/** \brief Parser stack implementation. 

 Stack implementation based on a std::stack. The behaviour of pop() had been
 slightly changed in order to get an error code if the stack is empty.
 The stack is used within the Parser both as a value stack and as an operator stack.

 \author (C) 2004, 2005 Ingo Berg 
*/
template <typename TValueType>
class ParserStack : private std::stack<TValueType, std::vector<TValueType> >
{
  private:
    /** \brief Type of the underlying stack implementation. */
    typedef std::stack<TValueType, std::vector<TValueType> > parent_type;

  public:	
	 
     //---------------------------------------------------------------------------
     ParserStack()
       :std::stack<TValueType, std::vector<TValueType> >()
     {
     }

     //---------------------------------------------------------------------------
     /** \brief Pop a value from the stack.
     
       Unlike the standard implementation this function will return the value that
       is going to be taken from the stack.

       \throw ParserException in case the stack is empty.
       \sa pop(int &a_iErrc)
     */
	   TValueType pop()
     {
       if (empty())
         throw ParserError("stack is empty.");

       TValueType el = top();
       parent_type::pop();
       return el;
     }

     /** \brief Push an object into the stack. 

         \param a_Val object to push into the stack.
         \throw nothrow
     */
     void push(const TValueType& a_Val) 
     { 
       parent_type::push(a_Val); 
     }

     /** \brief Return the number of stored elements. */
     unsigned size() const
     { 
       return (unsigned)parent_type::size(); 
     }

     /** \brief Returns true if stack is empty false otherwise. */
     bool empty() const
     {
       return parent_type::size()==0; 
     }
     
     /** \brief Return reference to the top object in the stack. 
     
         The top object is the one pushed most recently.
     */
     TValueType& top() 
     { 
       return parent_type::top(); 
     }
};

} // namespace MathUtils

#endif
