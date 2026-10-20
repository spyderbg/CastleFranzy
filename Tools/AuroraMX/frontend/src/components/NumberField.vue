<script setup lang="ts">
import { hex } from "../../../shared/sprite";
const p = defineProps<{
  label: string;
  value: number;
  hexadecimal?: boolean;
}>();
const emit = defineEmits<{ change: [value: number] }>();
function commit(event: FocusEvent) {
  const input = event.target as HTMLInputElement;
  const value = Number(input.value);
  if (Number.isInteger(value) && value !== p.value) emit("change", value);
  else input.value = p.hexadecimal ? hex(p.value) : String(p.value);
}
</script>
<template>
  <label class="field"
    ><span>{{ p.label }}</span>
    <input
      :key="p.value"
      :aria-label="p.label"
      :type="p.hexadecimal ? 'text' : 'number'"
      :value="p.hexadecimal ? hex(p.value) : p.value"
      @blur="commit"
      @keydown.enter="($event.target as HTMLInputElement).blur()"
    />
  </label>
</template>
