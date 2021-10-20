<?rsa version="1.0" encoding="utf-8"?>
<Configuration>
	<Product Id="Crypto-C ME">
		<Version>RSA BSAFE Crypto-C ME 4.1.2.0 2015-11-30 15:09</Version>
		<ReleaseDate>2015-11-30 15:09</ReleaseDate>
		<ExpDate>""</ExpDate>
		<Copyright>
			Copyright (C) RSA
		</Copyright>
		<Library Id="master">cryptocme</Library>
	</Product>
	<Runtime Id="runtime">
		<LoadOrder>
			<Library Id="ccme_error_info">ccme_error_info</Library>
			<Library Id="ccme_aux_entropy">ccme_aux_entropy</Library>
			<Library Id="ccme_base">ccme_base</Library>
			<Library Id="ccme_asym">ccme_asym</Library>
			<Library Id="ccme_ecc_accel_fips">ccme_ecc_accel_fips</Library>
			<Library Id="ccme_ecc">ccme_ecc</Library>
			<Library Id="ccme_base_non_fips">ccme_base_non_fips</Library>
			<Library Id="ccme_ecc_accel_non_fips">ccme_ecc_accel_non_fips</Library>
			<Library Id="ccme_ecc_non_fips">ccme_ecc_non_fips</Library>
		</LoadOrder>
		<StartupConfig>
			<SelfTest>OnLoad</SelfTest>
		</StartupConfig>
	</Runtime>
	<Signature URI="#ccme_error_info" Algorithm="FIPS140_INTEGRITY">MDwCHFyxmbnnOftp6WbT93iBGf3npwsh4eisCwgLlrYCHCbWzO14YrRhlUV1LCVQV8aJqHtNLj/r2rZoDfA=</Signature>
	<Signature URI="#ccme_aux_entropy" Algorithm="FIPS140_INTEGRITY">MD0CHQCShfR7XzRcx0XYk2b9eJSbIa0Uz5x+oUz+qEVKAhxMKbSRLA4mtKX369wPc5g0UuLYBgL4ODjH+R/u</Signature>
	<Signature URI="#ccme_base" Algorithm="FIPS140_INTEGRITY">MDwCHC8ba9YXZUsYtnBX+jaDhmtDIbOmyiVu4Iy25DMCHAHLQGkNBS3PDHcEgcERC6SpS3A4i0gkI4iu2BQ=</Signature>
	<Signature URI="#ccme_asym" Algorithm="FIPS140_INTEGRITY">MD0CHA2lfSaV1tNFZWEc1chre6JOsAjJWuLeVg+/WUkCHQCHuJ8yeHdoIwODysvUjiM1O5r9nJAve1mBlWL9</Signature>
	<Signature URI="#ccme_ecc_accel_fips" Algorithm="FIPS140_INTEGRITY">MDwCHGZa9q9dpI/1xndky8keKDPfxndfe8Jh20rmnIoCHAHG7I0PPJ7d2kBbQsAqfVqvapgD10AhLvtWdNM=</Signature>
	<Signature URI="#ccme_ecc" Algorithm="FIPS140_INTEGRITY">MD0CHGO3b47mhxwgNL4PxJcqHgzhCAX0KXNPQiBsa/ICHQCzN0UKoRrgTsLwuNxe4NHMWmAnEiWFXzuDbY0V</Signature>
	<Signature URI="#ccme_base_non_fips" Algorithm="FIPS140_INTEGRITY">MD0CHQCGBtWlBbcTpclA/Ukh7PS/256Nue3GOJdpQZfIAhxr2OZlLE27GqXzOYJ08kGnpTGnVqOTevIUrZcd</Signature>
	<Signature URI="#ccme_ecc_accel_non_fips" Algorithm="FIPS140_INTEGRITY">MDwCHBBN+aymQ7uY/dk3F/KIzW1yn5tAsq3Q1aSnWhQCHGF8+CWdJbySr3j96hQV4PExP4ngl5eeMvaKJhk=</Signature>
	<Signature URI="#ccme_ecc_non_fips" Algorithm="FIPS140_INTEGRITY">MDwCHGR0eErPcvMm1dK/ULRxhD/34eEw968fyAthoV8CHES7PkGxntD/Da4V9U6GEXr7tM2lQExpeGYKXXA=</Signature>
	<Signature URI="#master" Algorithm="FIPS140_INTEGRITY">MD0CHBweS6K3rRRKWJYU34fRNqQTDNNvqsgTbBZTGN0CHQChL44mB0WiqCmc6Z/3drtbrL+Gwxy0hu6GAAst</Signature>
	<Signature URI="#Crypto-C ME" Algorithm="FIPS140_INTEGRITY">MDwCHGzTBsZXCJGizlkCrTmKGd/bJ3fgPM7iuNRu8xQCHFeANjwnnGzTJvhC1PuSR3SH6TTp/dceK4UmiCU=</Signature>
	<Signature URI="#runtime" Algorithm="FIPS140_INTEGRITY">MDwCHAqULDK6xCmf7b+G+B2qoP2JxUprz893evk/B40CHBinnK0imwx0Qc4wG2LerY0eyyQxGFHqvVS9ppE=</Signature>
</Configuration>

