<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>
	
	<xsl:template match="/testsuite">
		<xsl:copy>
			<xsl:copy-of select="@failures|@disabled|@errors|@time|@name" />
			<xsl:attribute name="tests">
				<xsl:value-of select="sum(./testsuite[(./*[@status!='notrun'])]/@tests)" />
			</xsl:attribute>
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="/testsuite/testsuite">
		<xsl:if test="./*[@status!='notrun']">
			<xsl:copy-of select="." />
		</xsl:if>
	</xsl:template>
</xsl:transform>
