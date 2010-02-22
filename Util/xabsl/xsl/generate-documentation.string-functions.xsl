<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:template name="replace-in-string">
		<!-- A nice template for "search and replace" on strings. 
	      Parameter original-string: the string to process
	      Parameter replace: the expression to be replaced.
	      Parameter with: the new string.
	      
	      Example: original-string: "1+3+6=10", replace: "+", with: "-" returns "1-3-6=10".   -->
		<xsl:param name="original-string"/>
		<xsl:param name="replace"/>
		<xsl:param name="with"/>
		<xsl:variable name="string-before-replace-pattern" select="substring-before($original-string,$replace)"/>
		<xsl:choose>
  		<xsl:when test="string-length($string-before-replace-pattern)=0">
	  		<xsl:value-of select="$original-string"/>
  		</xsl:when>
	  	<xsl:otherwise>
		  	<xsl:value-of select="$string-before-replace-pattern"/>
			  <xsl:value-of select="$with"/>
  			<xsl:call-template name="replace-in-string">
	  			<xsl:with-param name="original-string" select="substring-after($original-string,$replace)"/>
		  		<xsl:with-param name="replace" select="$replace"/>
			  	<xsl:with-param name="with" select="$with"/>
  			</xsl:call-template>
	  	</xsl:otherwise>
	  </xsl:choose>
	</xsl:template>
	<xsl:template name="add-line-breaks">
		<!-- A template that adds "\n" line breaks to a string after a minimum length of characters.
  	      Parameter original-string: the string to process
  	      Parameter min-length: the minimum length after that a line break is inserted 
  	
   	      Example: original-string: "Tom doesn't like to eat cats", min-length: "10" returns "Tom doesn't\nlike to eat\ncats."-->
		<xsl:param name="original-string"/>
		<xsl:param name="min-length"/>
		<xsl:variable name="first-sub-string" select="substring-before($original-string,substring($original-string,$min-length+1))"/>
		<xsl:if test="string-length($first-sub-string)=0">
			<xsl:value-of select="$original-string"/>
		</xsl:if>
		<xsl:if test="string-length($first-sub-string)>0">
			<xsl:variable name="second-sub-string" select="substring-before(substring($original-string,$min-length+1),' ')"/>
			<xsl:variable name="third-sub-string" select="substring-after(substring($original-string,$min-length+1),' ')"/>
			<xsl:if test="string-length($third-sub-string)=0">
				<xsl:value-of select="$original-string"/>
			</xsl:if>
			<xsl:if test="string-length($third-sub-string)>0">
				<xsl:value-of select="$first-sub-string"/>
				<xsl:value-of select="$second-sub-string"/>\n<xsl:call-template name="add-line-breaks">
					<xsl:with-param name="original-string" select="$third-sub-string"/>
					<xsl:with-param name="min-length" select="$min-length"/>
				</xsl:call-template>
			</xsl:if>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>
