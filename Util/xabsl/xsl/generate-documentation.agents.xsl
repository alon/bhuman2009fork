<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL" xmlns:dotml="http://www.martin-loetzsch.de/DOTML">
	<xsl:import href="generate-documentation.option-tree.xsl"/>
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="xml" indent="yes"/>
	<xsl:param name="option-tree-xml"/>
	<xsl:template match="xabsl:agent-collection">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: Agents</title>
				<link rel="stylesheet" type="text/css" href="styles.css">
					<xsl:text> </xsl:text>
				</link>
			</head>
			<body>
				<table border="0" cellpadding="0" cellspacing="0">
					<tr>
						<td class="menu-cell">
							<table border="0" cellspacing="0" cellpadding="1">
								<xsl:call-template name="menu-xabsl2-logo"/>
								<xsl:call-template name="menu-entry-index-linked"/>
								<xsl:call-template name="menu-entry-agents"/>
								<xsl:call-template name="menu-agents"/>
								<xsl:call-template name="menu-entry-symbols-linked"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>Agents</h1>
							<xsl:apply-templates select="xabsl:agent"/>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="xabsl:agent">
		<h2 id="agent.{@id}">
			<xsl:value-of select="@title"/>
		</h2>
		<p>
			<xsl:value-of select="@description"/>
		</p>
		<p>Option Graph:<br/>
			<dotml:graph>
				<xsl:attribute name="file-name">svg/agent_<xsl:value-of select="@id"/></xsl:attribute>
				<xsl:call-template name="paint-option-tree">
					<xsl:with-param name="root-option" select="@root-option"/>
					<xsl:with-param name="option-tree-xml" select="$option-tree-xml"/>
				</xsl:call-template>
			</dotml:graph>
		</p>
	</xsl:template>
</xsl:stylesheet>
