<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:import href="generate-documentation.parameters.xsl"/>
	<xsl:import href="generate-documentation.menu.xsl"/>
	<xsl:output method="html" indent="yes"/>
	<xsl:template match="xabsl:symbols">
		<html>
			<head>
				<title>XABSL2 Behavior Documentation: <xsl:value-of select="@title"/>
				</title>
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
								<xsl:call-template name="menu-entry-agents-linked"/>
								<xsl:call-template name="menu-entry-symbols-linked"/>
								<xsl:call-template name="menu-symbol"/>
								<xsl:call-template name="menu-entry-basic-behaviors-linked"/>
								<xsl:call-template name="menu-entry-options-linked"/>
							</table>
						</td>
						<td class="main-area">
							<h1>
								<xsl:value-of select="@title"/>
							</h1>
							<p>
							<xsl:value-of select="@description"></xsl:value-of>
							</p>
							<p>The formalized behavior references a variety of input and output symbols, which can stand for variables or functions of the agent's software. Constants are constant decimal values.</p>
		<xsl:if test="xabsl:enumeration">
							<h2 id="enumerations">Enumerations</h2>
							<table border="0" cellpadding="0" cellspacing="6">
								<tr>
									<td class="tablehead">Name</td>
									<td class="tablehead">Elements</td>
									<td class="tablehead">Description</td>
								</tr>
								<xsl:apply-templates select="xabsl:enumeration"/>
							</table>
		</xsl:if>
		<xsl:if test="xabsl:decimal-input-symbol | xabsl:boolean-input-symbol | xabsl:enumerated-input-symbol">

							<h2 id="input-symbols">Input Symbols</h2>
							<table border="0" cellpadding="0" cellspacing="6">
								<tr>
									<td class="tablehead">Name</td>
									<td class="tablehead">Type</td>
									<td class="tablehead">Measure</td>
									<td class="tablehead">Description / Parameters</td>
								</tr>
								<xsl:apply-templates select="xabsl:decimal-input-symbol | xabsl:boolean-input-symbol | xabsl:enumerated-input-symbol"/>
							</table></xsl:if>
									<xsl:if test="xabsl:decimal-output-symbol | xabsl:boolean-output-symbol | xabsl:enumerated-output-symbol">

							
							<h2 id="output-symbols">Output Symbols</h2>
							<table border="0" cellpadding="0" cellspacing="6">
								<tr>
									<td class="tablehead">Name</td>
									<td class="tablehead">Type</td>
									<td class="tablehead">Measure</td>
									<td class="tablehead">Description</td>
								</tr>
								<xsl:apply-templates select="xabsl:decimal-output-symbol | xabsl:boolean-output-symbol | xabsl:enumerated-output-symbol"/>
							</table>
							
							</xsl:if>
		<xsl:if test="xabsl:constant">
							<h2 id="constants">Constants</h2>
							<table border="0" cellpadding="0" cellspacing="6">
								<tr>
									<td class="tablehead">Name</td>
									<td class="tablehead">Value</td>
									<td class="tablehead">Measure</td>
									<td class="tablehead">Description</td>
								</tr>
								<xsl:apply-templates select="xabsl:constant"/>
							</table>
							
							</xsl:if>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="xabsl:enumeration">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td class="italic">
				<xsl:for-each select="xabsl:enum-element">
					<xsl:value-of select="@name"/>
					<br/>
				</xsl:for-each>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:decimal-input-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>decimal</td>
			<td class="italic">
				<xsl:value-of select="@measure"/>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
		<tr>
			<td colspan="3">&#160;</td>
			<td>
				<xsl:call-template name="parameters"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:boolean-input-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>boolean</td>
			<td class="italic">true/ false</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
		<tr>
			<td colspan="3">&#160;</td>
			<td>
				<xsl:call-template name="parameters"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:decimal-output-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>decimal</td>
			<td class="italic">
				<xsl:value-of select="@measure"/>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:boolean-output-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>boolean</td>
			<td class="italic">true/ false</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:enumerated-input-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>enumerated</td>
			<td class="bold">
				<xsl:value-of select="@enumeration"/>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
		<tr>
			<td colspan="3">&#160;</td>
			<td>
				<xsl:call-template name="parameters"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:enumerated-output-symbol">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>enumerated</td>
			<td class="bold">
				<xsl:value-of select="@enumeration"/>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
	</xsl:template>
	<xsl:template match="xabsl:constant">
		<tr id="{@name}">
			<td class="bold">
				<xsl:value-of select="@name"/>
			</td>
			<td>
				<xsl:value-of select="@value"/>
			</td>
			<td class="italic">
				<xsl:value-of select="@measure"/>
			</td>
			<td>
				<xsl:value-of select="@description"/>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
