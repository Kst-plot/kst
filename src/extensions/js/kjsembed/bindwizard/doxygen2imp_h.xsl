<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:template match="/doxygen/compounddef">

<xsl:variable name="clazz" select="compoundname" />
<xsl:variable name="consmeth" select="sectiondef/memberdef[@kind='function' and @prot='public']" />

#ifndef KJSEMBED_<xsl:value-of select="$clazz" />_H
#define KJSEMBED_<xsl:value-of select="$clazz" />_H

#include &lt;kjs/interpreter.h&gt;
#include &lt;kjs/object.h&gt;

#include &lt;kjsembed/jsobjectproxy_imp.h&gt;

class <xsl:value-of select="$clazz" />;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for <xsl:value-of select="$clazz" /> methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class <xsl:value-of select="$clazz" />Imp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
<xsl:for-each select="$consmeth">
  <xsl:choose>
    <xsl:when test="name = $clazz">
     <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="cons_id">Constructor_<xsl:value-of select="$cons_name" /></xsl:variable>
       <xsl:text>       </xsl:text><xsl:value-of select="$cons_id" />,
</xsl:when>
  </xsl:choose>
</xsl:for-each>
       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {
<xsl:for-each select="$consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
  <xsl:choose>
    <xsl:when test="not(starts-with( name, 'operator' ) or starts-with( name, '~' ) or starts-with( name, $clazz ))">
       Method_<xsl:value-of select="$method_name" />,</xsl:when>
  </xsl:choose>
</xsl:for-each>
       Method_Last = -1
   };

   <xsl:value-of select="$clazz" />Imp( KJS::ExecState *exec, int id, bool constructor=false );
   ~<xsl:value-of select="$clazz" />Imp();

   /**
    * Adds the static bindings for this class to the specified Object. The
    * static bindings are the static methods and the enums of this class.
    */
   static void addStaticBindings( KJS::ExecState *exec, KJS::Object &amp;object );

   /**
    * Adds the bindings for this class to the specified Object. The bindings
    * added are the instance methods of this class.
    */
   static void addBindings( KJS::ExecState *exec, KJS::Object &amp;object );

   static <xsl:value-of select="$clazz" /> *to<xsl:value-of select="$clazz" />( KJS::Object &amp;object );

   //
   // Constructors implemented by this class.
   //
<xsl:for-each select="$consmeth">
    <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
  <xsl:choose>
    <xsl:when test="name = $clazz">
   KJS::Object <xsl:value-of select="$cons_name" />( KJS::ExecState *exec, const KJS::List &amp;args );</xsl:when>
  </xsl:choose>
</xsl:for-each>

   //
   // Methods implemented by this class.
   //
<xsl:for-each select="$consmeth">
   <xsl:variable name="method_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
  <xsl:choose>
   <xsl:when test="not(starts-with( name, 'operator' ) or starts-with( name, '~' ) or starts-with( name, $clazz ))">
   KJS::Value <xsl:value-of select="$method_name" />( KJS::ExecState *exec, KJS::Object &amp;obj, const KJS::List &amp;args );</xsl:when>
  </xsl:choose>
</xsl:for-each>

   //
   // JS binding code.
   //

   /** Returns true iff this object implements the construct function. */
   virtual bool implementsConstruct() const { return cons; }

   /** Invokes the construct function. */
   virtual KJS::Object construct( KJS::ExecState *exec, const KJS::List &amp;args );

   /** Returns true iff this object implements the call function. */
   virtual bool implementsCall() const { return !cons; }

   /** Invokes the call function. */
   virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &amp;self, const KJS::List &amp;args );

private:
   <xsl:value-of select="$clazz" /> *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_<xsl:value-of select="compoundname" />_H

// Local Variables:
// c-basic-offset: 4
// End:

</xsl:template>
</xsl:stylesheet>
