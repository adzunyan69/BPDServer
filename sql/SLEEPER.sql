SELECT
/* Запрос по мостам */
c.UP_NOM    as KOD,
c.NOM       as PUT,
"SLP"      as OBJ,
d.TYPE         as NUMB,
n.TYPE      as NAME,
d.BEGIN_KM  as B_KM,
d.BEGIN_M   as B_M,
0           as O_KM,
0   	    as O_M,
d.END_KM    as E_KM,
d.END_M     as E_M,
0           as LEN 		


FROM WAY c , GB d, SPR_GB n

/* WHERE c.assetnum = '110000123030' */
WHERE c.up_nom = :DIR_CODE and c.nom = :TRACK_NUM
/* Параметр  c.assetnum = :P из папспорта проезда Atape */
/* and c.assetnum = :P */
and d.up_nom   = c.up_nom
and d.put_nom  = c.nom
and d.type=n.id

order by
KOD,
PUT,
OBJ,
B_KM,
B_M