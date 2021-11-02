SELECT
/* Запрос по ASSETNUM */
UP_NOM    as KOD,
NOM       as PUT

FROM WAY  

WHERE assetnum = :ASSET_NUM
/* WHERE c.assetnum = :Parametr */