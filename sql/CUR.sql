SELECT
/* Запрос по путепроводам */
c.UP_NOM    as KOD,
c.NOM       as PUT,
'CUR'       as OBJ,
0           as NUMB,
'Кривая'    as NAME,
d.KM        as B_KM,
d.M         as B_M,
d.RIHT      as O_KM,
d.IZN  	    as O_M,
0           as E_KM,
0           as E_M,
0           as LEN 		


FROM WAY c , PLANPOINTS d

/* WHERE c.assetnum = :ASSET_NUM */
WHERE c.up_nom = :DIR_CODE and c.nom = :TRACK_NUM
/* Параметр  c.assetnum = :P из папспорта проезда Atape */
/* and c.assetnum = :P */
and d.up_nom   = c.up_nom
and d.put_nom  = c.nom
and d.type = 0

order by
KOD,
PUT,
OBJ,
B_KM,
B_M