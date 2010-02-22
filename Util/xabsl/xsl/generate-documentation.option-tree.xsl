<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.string-functions.xsl"/>
	<xsl:output method="xml" indent="yes"/>
	<xsl:key name="options" match="xabsl:agent-collection/xabsl:options/xabsl:option" use="@name"/>
	<xsl:key name="basic-behaviors" match="xabsl:agent-collection/xabsl:options/xabsl:option/xabsl:basic-behaviors/xabsl:basic-behavior" use="@name"/>
	<xsl:template match="xabsl:agent-collection">
		<xabsl:option-trees>
			<xsl:for-each select="xabsl:options/xabsl:option">
				<xabsl:option-tree root-option="{@name}">
					<xsl:call-template name="create-option-tree"/>
				</xabsl:option-tree>
			</xsl:for-each>
		</xabsl:option-trees>
	</xsl:template>
	<xsl:template name="create-option-tree">
		<xabsl:o-n name="{@name}">
			<xsl:for-each select="xabsl:common-action/xabsl:subsequent-option | xabsl:state/xabsl:subsequent-option">
				<xsl:variable name="ref" select="@ref"/>
				<xsl:if test="not(../preceding-sibling::xabsl:state/xabsl:subsequent-option[@ref=$ref])">
					<xabsl:e2o to="{@ref}"/>
				</xsl:if>
			</xsl:for-each>
			<xsl:for-each select="xabsl:common-action/xabsl:subsequent-basic-behavior | xabsl:state/xabsl:subsequent-basic-behavior">
				<xsl:variable name="ref" select="@ref"/>
				<xsl:if test="not(../preceding-sibling::xabsl:state/xabsl:subsequent-basic-behavior[@ref=$ref])">
					<xabsl:e2b to="{@ref}"/>
				</xsl:if>
			</xsl:for-each>
		</xabsl:o-n>
		<xsl:for-each select="xabsl:common-action/xabsl:subsequent-option | xabsl:state/xabsl:subsequent-option">
			<xsl:variable name="ref" select="@ref"/>
			<xsl:if test="not(../preceding-sibling::xabsl:state/xabsl:subsequent-option[@ref=$ref])">
				<xsl:for-each select="key('options',@ref)[1]">
					<xsl:call-template name="create-option-tree"/>
				</xsl:for-each>
			</xsl:if>
		</xsl:for-each>
		<xsl:for-each select="xabsl:common-action/xabsl:subsequent-basic-behavior | xabsl:state/xabsl:subsequent-basic-behavior">
			<xsl:variable name="ref" select="@ref"/>
			<xsl:if test="not(../preceding-sibling::xabsl:state/xabsl:subsequent-basic-behavior[@ref=$ref])">
				<xabsl:b-n name="{@ref}" g="{key('basic-behaviors',@ref)[1]/../@id}"/>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="paint-option-tree">
		<xsl:param name="root-option"/>
		<xsl:param name="dont-paint-option"/>
		<xsl:param name="option-tree-xml"/>
		<xsl:for-each select="document($option-tree-xml)/xabsl:option-trees/xabsl:option-tree[@root-option=$root-option]">
			<xsl:for-each select="xabsl:o-n[@name!=$dont-paint-option]">
				<xsl:variable name="name" select="@name"/>
				<xsl:if test="count(preceding-sibling::xabsl:o-n[@name=$name])=0">
					<dotml:node fontname="Arial" fontcolor="#005A9C" fontsize="9" shape="box" URL="../option.{@name}.html" fillcolor="#FFFFFF" style="filled">
						<xsl:attribute name="id">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
						<xsl:attribute name="label"><xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">\n</xsl:with-param></xsl:call-template></xsl:attribute>
					</dotml:node>
				</xsl:if>
			</xsl:for-each>
			<dotml:sub-graph rank="sink">
				<xsl:for-each select="xabsl:b-n">
					<xsl:variable name="name" select="@name"/>
					<xsl:if test="count(preceding-sibling::xabsl:b-n[@name=$name])=0">
						<dotml:node fontname="Arial" fontcolor="#005A9C" fontsize="9" URL="../basic-behaviors.{@g}.html#{@name}" style="filled" fillcolor="#FFFFFF">
							<xsl:attribute name="id">basic_behavior_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
							<xsl:attribute name="label"><xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">\n</xsl:with-param></xsl:call-template></xsl:attribute>
						</dotml:node>
					</xsl:if>
				</xsl:for-each>
			</dotml:sub-graph>
			<xsl:for-each select="xabsl:o-n[@name!=$dont-paint-option]">
				<xsl:variable name="name" select="@name"/>
				<xsl:if test="count(preceding-sibling::xabsl:o-n[@name=$name])=0">
					<xsl:for-each select="xabsl:e2o">
						<dotml:edge arrowsize="0.8" color="#808080">
							<xsl:attribute name="from">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="../@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
							<xsl:attribute name="to">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@to"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
						</dotml:edge>
					</xsl:for-each>
					<xsl:for-each select="xabsl:e2b">
						<dotml:edge arrowsize="0.8" color="#808080" minlen="3">
							<xsl:attribute name="from">option_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="../@name"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
							<xsl:attribute name="to">basic_behavior_<xsl:call-template name="replace-in-string"><xsl:with-param name="original-string" select="@to"/><xsl:with-param name="replace">-</xsl:with-param><xsl:with-param name="with">_</xsl:with-param></xsl:call-template></xsl:attribute>
						</dotml:edge>
					</xsl:for-each>
				</xsl:if>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>
