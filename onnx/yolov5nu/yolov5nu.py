from ultralytics import YOLO

yolov5nu = "yolov5nu"
source_dir = "../imgs/"
results_dir = "../../../res/"

# 1. Загружаем модель
model = YOLO(yolov5nu + ".pt")

# 2. Запускаем детекцию на изображении.
# Это может быть путь к локальному файлу или URL.
results = model(
    source=source_dir,  			# входная папка
    project=results_dir,         	# базовая папка для всех результатов
    name=yolov5nu,           		# подпапка для этого запуска
    save=True,                      # сохранять изображения
    save_txt=True,                  # сохранять координаты в txt
    save_conf=True                  # сохранять уверенность модели
)

# 3. Работаем с результатами.
# Результатов может быть несколько, если на вход подали папку с фото.
# Показать изображение с рамками в отдельном окне - results[0].show()
print(f"Результаты сохранены в: {results[0].save_dir}")

# 4. Экспорт модели в формате onnx
# path = model.export(format="onnx")
# print(f"Модель успешно экспортирована в: {path}")
