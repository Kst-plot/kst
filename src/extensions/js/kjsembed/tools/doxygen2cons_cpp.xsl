<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />

<xsl:template match="/doxygen/compounddef">
<xsl:variable name="clazz" select="compoundname" />
<xsl:variable name="consmeth" select="sectiondef/memberdef[@kind='function' and @prot='public']" />

#include &lt;klocale.h>
#include &lt;kjs/object.h>

#include &lt;<xsl:value-of select="includes" />>
#include "<xsl:value-of select="substring-before(includes,'.h')"/>_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

<!-- Select and invoke the correct constructor. -->
KJS::Object <xsl:value-of select="$clazz" />Cons::construct( KJS::ExecState *exec, KJS::Object &amp;obj, const KJS::List &amp;args )
{
   switch( id ) {
<xsl:for-each select="$consmeth">
  <xsl:choose>
    <xsl:when test="name = $clazz">
     <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="cons_id">Constructor_<xsl:value-of select="$cons_name" /></xsl:variable>
         case <xsl:value-of select="$cons_id" />:
             return <xsl:value-of select="$cons_name" />( exec, obj, args );
             break;
    </xsl:when>
  </xsl:choose>
</xsl:for-each>
         default:
             break;
    }

    QString msg = i18n("<xsl:value-of select="$clazz" />Cons has no constructor with id '%1'").arg(id);
    KJS::Object err = KJS::Error::create( exec, KJS::ReferenceError, msg.utf8() );
    exec->setException( err );
    return err;
}

<!-- Implementations of the constructors. -->
<xsl:for-each select="$consmeth">
  <xsl:choose>
    <xsl:when test="name = $clazz">
     <xsl:variable name="cons_name"><xsl:value-of select="name" />_<xsl:value-of select="position()" /></xsl:variable>
   <xsl:variable name="cons_id">Constructor_<xsl:value-of select="$cons_name" /></xsl:variable>
KJS::Object <xsl:value-of select="$clazz" />Cons::<xsl:value-of select="$cons_name" />( KJS::ExecState *exec, KJS::Object &amp;obj, const KJS::List &amp;args )
{
    <xsl:for-each select="param/type">
      <xsl:choose>
        <xsl:when test=". = 'QString'">
    QString arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).qstring() : QString::null;
        </xsl:when>
        <xsl:when test=". = 'const QString &amp;'">
    QString arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).qstring() : QString::null;
        </xsl:when>
        <xsl:when test=". = 'const char *'">
    const char *arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toString(exec).ascii() : 0;
        </xsl:when>
        <xsl:when test=". = 'int'">
    int arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'uint'">
    uint arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'double'">
    double arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toInteger(exec) : -1;
        </xsl:when>
        <xsl:when test=". = 'bool'">
    bool arg<xsl:value-of select="position()-1" /> = (args.size() >= <xsl:value-of select="position()" />) ? args[<xsl:value-of select="position()-1" />].toBoolean(exec) : false;
        </xsl:when>
        <xsl:when test=". = 'const QStringList &amp;'">
    QStringList arg<xsl:value-of select="position()-1" />;
    if ( args.size() >= <xsl:value-of select="position()" /> ) {
      // TODO: populate the list
    }
        </xsl:when>
        <xsl:otherwise>
    // Unsupported parameter <xsl:value-of select="." />
    return KJS::Value();
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
}
    </xsl:when>
  </xsl:choose>
</xsl:for-each>

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

</xsl:template>
</xsl:stylesheet>
