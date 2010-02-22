<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xabsl="http://www.ki.informatik.hu-berlin.de/XABSL">
	<xsl:output method="xml" indent="no"/>
	<xsl:template name="menu-xabsl2-logo">
		<tr>
			<td colspan="2" class="xabsl-2-logo-big">
				<a href="http://www.ki.informatik.hu-berlin.de/XABSL" title="The XABSL Web Site">&gt;xabsl 2</a>
			</td>
		</tr>
		<tr>
			<td colspan="2" class="xabsl-2-logo-small">Behavior Documentation</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-agents-linked">
		<tr>
			<td colspan="2" class="menu-title">
				<a href="agents.html" title="All available agents">&gt;Agents</a>
			</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-agents">
		<tr>
			<td colspan="2" class="menu-title">&gt;Agents</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-index-linked">
		<tr>
			<td colspan="2" class="menu-item">
				<br/>&gt;<a href="index.html" title="Start Page">Index</a>
			</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-index">
		<tr>
			<td colspan="2" class="menu-item">
				<br/>&gt;Index</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-symbols-linked">
		<tr>
			<td colspan="2" class="menu-title">
				<a href="symbols.html" title="All available symbols">&gt;Symbols</a>
			</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-symbols">
		<tr>
			<td colspan="2" class="menu-title">&gt;Symbols</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-basic-behaviors-linked">
		<tr>
			<td colspan="2" class="menu-title">
				<a href="basic-behaviors.html" title="All available basic behaviors">&gt;Basic Behaviors</a>
			</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-basic-behaviors">
		<tr>
			<td colspan="2" class="menu-title">&gt;Basic Behaviors</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-options-linked">
		<tr>
			<td colspan="2" class="menu-title">
				<a href="options.html" title="All available options">&gt;Options</a>
			</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-entry-options">
		<tr>
			<td colspan="2" class="menu-title">&gt;Options</td>
		</tr>
	</xsl:template>
	<xsl:template name="menu-basic-behaviors">
		<xsl:for-each select="xabsl:options/xabsl:option/xabsl:basic-behaviors[count(key('basic-behaviors',@id)[1] | .)=1]">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<a href="basic-behaviors.{@id}.html" title="{@description}">
						<xsl:value-of select="@title"/>
					</a>
				</td>
			</tr>
			<xsl:for-each select="xabsl:basic-behavior">
				<tr>
					<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="basic-behaviors.{../@id}.html#{@name}" title="{@description}">
							<xsl:value-of select="@name"/>
						</a>
					</td>
				</tr>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="menu-basic-behavior">
		<tr>
			<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<xsl:value-of select="@title"/>
			</td>
		</tr>
		<xsl:for-each select="xabsl:basic-behavior">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="basic-behaviors.{../@id}.html#{@name}" title="{@description}">
						<xsl:value-of select="@name"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="menu-options">
		<xsl:for-each select="xabsl:option-definition">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<a href="option.{@name}.html" class="menu-item" title="{@description}">
						<xsl:value-of select="@name"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="menu-symbols">
		<xsl:for-each select="xabsl:options/xabsl:option/xabsl:symbols[count(key('symbols',@id)[1] | .)=1]">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<a href="symbols.{@id}.html" title="{@description}">
						<xsl:value-of select="@title"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="menu-agents">
		<xsl:for-each select="xabsl:agent">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<a href="agents.html#agent.{@id}" title="{@description}">
						<xsl:value-of select="@title"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
	<xsl:template name="menu-symbol">
		<tr>
			<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<xsl:value-of select="@title"/>
			</td>
		</tr>
		<xsl:if test="xabsl:decimal-input-symbol | xabsl:boolean-input-symbol | xabsl:enumerated-input-symbol">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="symbols.{@id}.html#input-symbols" title="All available input symbols">Input Symbols</a>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="xabsl:decimal-input-function">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="symbols.{@id}.html#input-functions" title="All available input functions">Input Functions</a>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="xabsl:enumerated-output-symbol">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="symbols.{@id}.html#output-symbols" title="All available output symbols">Output Symbols</a>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="xabsl:constant">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="symbols.{@id}.html#constants" title="All available constants">Constants</a>
				</td>
			</tr>
		</xsl:if>
	</xsl:template>
	<xsl:template name="menu-option">
		<xsl:variable name="name" select="@name"/>
		<xsl:for-each select="xabsl:option-definitions/xabsl:option-definition[@name = $name]">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&gt;&gt;&#160;&#160;<xsl:value-of select="@name"/>
				</td>
			</tr>
		</xsl:for-each>
		<xsl:for-each select="xabsl:state">
			<tr>
				<td class="menu-item" nowrap="nowrap" colspan="2">&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&gt;&gt;&gt;&#160;&#160;<a href="option.{../@name}.html#state_{@name}" title="State {@name}">
						<xsl:value-of select="@name"/>
					</a>
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
	<xsl:template match="xabsl:agent-collection">
		<table border="0" cellspacing="0" cellpadding="1">
			<!--<tr>
				<td colspan="2">
					<img src="Xabsl2Logo.gif"/>
				</td>
			</tr>-->
			<tr>
				<td colspan="2" class="xabsl-2-logo-big">>xabsl 2</td>
			</tr>
			<tr>
				<td colspan="2" class="xabsl-2-logo-small">Behavior Documentation</td>
			</tr>
			<tr>
				<td colspan="2" class="menu-item">
					<br/>><a href="index.html" title="Start Page">Index</a>
				</td>
			</tr>
			<tr>
				<td colspan="2" class="menu-title">>Agents:</td>
			</tr>
			<tr>
				<xsl:for-each select="xabsl:agent">
					<tr>
						<td class="menu-item" nowrap="nowrap">>></td>
						<td>
							<a href="agent.{@id}.html" class="menu-item" title="{@description}">
								<xsl:value-of select="@title"/>
							</a>
						</td>
					</tr>
				</xsl:for-each>
			</tr>
			<tr>
				<td colspan="2" class="menu-title">>Symbols:</td>
			</tr>
			<tr>
				<xsl:for-each select="xabsl:agent/xabsl:symbols">
					<xsl:variable name="title" select="@title"/>
					<xsl:if test="count(preceding::xabsl:symbols[@title=$title])=0">
						<tr>
							<td class="menu-item" nowrap="nowrap">>></td>
							<td>
								<a href="symbols.{@id}.html" class="menu-item" title="{@description}">
									<xsl:value-of select="@title"/>
								</a>
							</td>
						</tr>
					</xsl:if>
				</xsl:for-each>
			</tr>
			<tr>
				<td colspan="2" class="menu-title">>Basic Behaviors:</td>
			</tr>
			<tr>
				<xsl:for-each select="xabsl:agent/xabsl:basic-behaviors">
					<xsl:variable name="title" select="@title"/>
					<xsl:if test="count(preceding::xabsl:basic-behaviors[@title=$title])=0">
						<tr>
							<td class="menu-item" nowrap="nowrap">>></td>
							<td>
								<a href="basic-behaviors.{@id}.html" class="menu-item" title="{@description}">
									<xsl:value-of select="@title"/>
								</a>
							</td>
						</tr>
					</xsl:if>
				</xsl:for-each>
			</tr>
			<tr>
				<td colspan="2" class="menu-title">>Options:</td>
			</tr>
			<tr>
				<xsl:for-each select="xabsl:agent/xabsl:option-definitions/xabsl:option-definition">
					<xsl:variable name="name" select="@name"/>
					<xsl:if test="count(preceding::xabsl:option-definition[@name=$name])=0">
						<tr>
							<td class="menu-item" nowrap="nowrap">>></td>
							<td>
								<a href="option.{@name}.html" class="menu-item" title="{@description}">
									<xsl:value-of select="@name"/>
								</a>
							</td>
						</tr>
					</xsl:if>
				</xsl:for-each>
			</tr>
		</table>
	</xsl:template>
</xsl:stylesheet>
